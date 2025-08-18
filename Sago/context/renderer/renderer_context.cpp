#include "renderer_context.h"

#include "core/io/log/log.h"

namespace Context::Renderer {

void RendererContext::InitImpl() {
	vk_context_ = std::make_unique<VulkanContext>(window_);
	//LogInfo("[Context][Renderer]: Current Thread Id: {}",std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

RendererContext::RendererContext(const Platform::AppWindow& window, const Controller::FrameRateController& controller) :
		window_(window), fpscontroller_(controller), thread_(&RendererContext::Tick, this) {
}

RendererContext::~RendererContext() noexcept {
	{
		PutEvent([this] {
			m_running = false;
		});
		LogInfo("[Context][Renderer]: RendererContext Quit");
	}
}

void RendererContext::PutEvent(callable_t&& callable) noexcept {
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		m_writeBuffer.emplace_back(std::move(callable));
	}
	m_condVar.notify_one();
}

void RendererContext::Tick() noexcept {
	Init(); //Delay Init

	std::vector<callable_t> readBuffer;
	while (m_running) {
		fpscontroller_.StartFrame();

		//Event Type
		auto event = queue_.pop();
		if (event) {
			HandleEvent(*event); 
			continue;
		}




		//Mutex
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condVar.wait(lock, [this] {
				return !m_running || !m_writeBuffer.empty();
			});
			if (!m_running) {
				break;
			}

			std::swap(readBuffer, m_writeBuffer);
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