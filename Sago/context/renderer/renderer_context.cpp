#include "renderer_context.h"

#include "core/io/log/log.h"

namespace Context::Renderer {

void RendererContext::Init() {
	vk_context_ = std::make_unique<VulkanContext>(window_);
    LogInfo("[Context][Renderer]: Current Thread Id: {}",std::hash<std::thread::id>{}(std::this_thread::get_id()));

}

RendererContext::RendererContext(const Platform::AppWindow& window, const Controller::FrameRateController& controller) :
		window_(window), fpscontroller_(controller), thread_(&RendererContext::Tick, this) {
}


void RendererContext::Tick() noexcept{
    Init(); //Delay Init


}


} //namespace Context::Renderer