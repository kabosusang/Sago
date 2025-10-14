#include "renderer_context.h"

#include "core/events/event_system.h"
#include "core/io/log/log.h"
#include "event/renderer_event_type.h"

#include <atomic>
#include <mutex>

namespace Context::Renderer {
using namespace Core::Event;
using namespace Context::Renderer::Event;

void RendererContext::InitImpl() {
	vk_context_ = std::make_unique<VulkanContext>(window_,editor_);
}

void RendererContext::ListenEventImpl() {
	auto& dispatch = EventSystem::Instance().GetRendererDispatcher();
	dispatch.subscribe<RenderNextFrameEvent>([&](const RenderNextFrameEvent& e) {
		this->work_pending_ = false;
	});

	dispatch.subscribe<SwapchainRecreateEvent>([&](const SwapchainRecreateEvent& e) {
		vk_context_->frame_buffer_resized_ = true;
	});

	dispatch.subscribe<RendererPauseEvent>([&](const RendererPauseEvent& e) {
		vk_context_->renderer_paused_.store(e.paused_, std::memory_order_release);
	});
}

RendererContext::RendererContext(const Platform::AppWindow& window,
	Platform::EditorUI& editor,const Controller::FrameRateController& controller) :
		window_(window),editor_(editor),fpscontroller_(controller) {
	//Start Thread
	running_.store(true, std::memory_order_relaxed);
	thread_ = std::jthread(&RendererContext::Tick, this);
	LogInfo("[Context][Renderer] RendererContext created");
	InitListenEvent();
}

RendererContext::~RendererContext() noexcept {
	running_.store(false, std::memory_order_release);
	{
		std::lock_guard lock(mutex_);
		work_pending_ = false;
	}
	work_cv_.notify_all();

	if (thread_.joinable()) {
		thread_.join();
	}
	LogInfo("[Context][Renderer][Destroy]: RendererContext Quit");
}

void RendererContext::PutEvent(callable_t&& callable) noexcept {
	int write_idx = write_index_.load(std::memory_order_relaxed);
	task_buffers_[write_idx].emplace_back(std::move(callable));
	{
		std::lock_guard lock(mutex_);
		work_pending_ = true;
	}
	work_cv_.notify_one();
}

void RendererContext::RequestFrame() noexcept {
	{
		std::lock_guard lock(mutex_);
		work_pending_ = true;
	}
	work_cv_.notify_one();
}

void RendererContext::Stop() noexcept {
	running_.store(false, std::memory_order_release);
	{
		std::lock_guard lock(mutex_);
		work_pending_ = true;
	}
	work_cv_.notify_all();

	if (thread_.joinable()) {
		thread_.join();
	}
	LogInfo("[RendererContext]: Renderer completely stopped");
}

void RendererContext::Tick() noexcept {
	Init(); //Delay Init
	LogInfo("[Context][Renderer] Render thread started");

	int current_read_index = 0;
	constexpr int busy_wait_count = 1000;
	constexpr int yield_count = 100;

	while (running_.load(std::memory_order_acquire)) {
		//fpscontroller_.StartFrame();
		//Event Bus
		EventSystem::Instance().ProcessUpToEvents<ThreadCategory::Renderer>(32);

		//Event Type
		if (auto event = event_queue_.try_pop(); event) {
			HandleEvent(*event);
			continue;
		}

		std::unique_lock lock(mutex_);
		work_cv_.wait(lock, [this] {
			return work_pending_ || !running_.load(std::memory_order_acquire);
		});

		if (!running_.load(std::memory_order_acquire)) {
			break;
		}

		work_pending_ = false;
		lock.unlock();

		// Task
		const int task_buffer_index = write_index_.load(std::memory_order_acquire);
		const int current_read_index = task_buffer_index ^ 1;
		write_index_.store(current_read_index, std::memory_order_release);
		ProcessTasks(task_buffer_index);

		//Renderer
		if (vk_context_) {
			vk_context_->Renderer();
		}
		//fpscontroller_.EndFrame();
	}
}

void RendererContext::HandleEvent(const Event& event) {
	switch (event) {
		case Event::kRendererFrame:
			LogInfo("[Context][Renderer] Renerer Event kRendererFrame: ");
			break;
	}
}

void RendererContext::ProcessTasks(int buffer_index) noexcept {
	auto& tasks = task_buffers_[buffer_index];
	if (tasks.empty()) {
		return;
	}

	const size_t task_count = tasks.size();
	for (size_t i = 0; i < task_count; ++i) {
		tasks[i]();
	}
	tasks.clear();
}

} //namespace Context::Renderer