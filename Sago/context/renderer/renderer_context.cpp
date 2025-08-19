#include "renderer_context.h"

#include "core/io/log/log.h"

namespace Context::Renderer {

void RendererContext::InitImpl() {
	vk_context_ = std::make_unique<VulkanContext>(window_);
	pipeline_   = std::make_unique<Pipeline>(vk_context_->GetDevice());
}

RendererContext::RendererContext(const Platform::AppWindow& window, const Controller::FrameRateController& controller) :
		window_(window), fpscontroller_(controller), thread_(&RendererContext::Tick, this) {
}

RendererContext::~RendererContext() noexcept {
	{
		PutEvent([this] {
			running_ = false;
		});
		LogInfo("[Context][Renderer][Destory]: RendererContext Quit");
	}
}

void RendererContext::PutEvent(callable_t&& callable) noexcept {
	{
		std::lock_guard<std::mutex> guard(mutex_);
		writebuffer_.emplace_back(std::move(callable));
	}
	cv_.notify_one();
}

void RendererContext::Tick() noexcept {
	Init(); //Delay Init

	std::vector<callable_t> readBuffer;
	while (running_) {
		fpscontroller_.StartFrame();

		//Event Type
		auto event = queue_.pop();
		if (event) {
			HandleEvent(*event);
			continue;
		}

		//Mutex
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this] {
				return !running_ || !writebuffer_.empty();
			});
			if (!running_) {
				break;
			}

			std::swap(readBuffer, writebuffer_);
		}
		for (auto& func : readBuffer) {
			func();
		}
		readBuffer.clear();

		fpscontroller_.EndFrame();
	}
}

void RendererContext::HandleEvent(const Event& event) {
	switch (event) {
		case Event::kRendererFrame:
			LogInfo("[Context][Renderer] Renerer Event kRendererFrame: ");
			break;
	}
}

} //namespace Context::Renderer