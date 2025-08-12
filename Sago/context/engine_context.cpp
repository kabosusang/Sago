#include "engine_context.h"

#include "vulkan_context.h"
#include "window/window_sdl.h"

namespace Context {

EngineContext::EngineContext() {
	using namespace Platform;

	window_ = std::make_unique<AppWindow>();
	if (!window_) {
		LogInfoDetaill("Context Window Create Error");
	}
	Init();
}

void EngineContext::Init() {
	vk_context_ = std::make_unique<VulkanContext>(*window_);


}

EngineContext::~EngineContext() {
	window_.reset();
}

void EngineContext::Quit() {
	window_->Quit();
}

} //namespace Context