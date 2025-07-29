#include "engine_context.h"

#include "window/window_sdl.h"
#include <memory>

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
	using namespace Driver::Vulkan;
	vkinitail_ = std::make_unique<VulkanInitializer>();
	vkdevice_  = std::make_unique<VulkanDevice>(*vkinitail_);
	vksurface_ = std::make_unique<VulkanSurface>(*window_,*vkinitail_);

}

EngineContext::~EngineContext() {
	window_.reset();
}

void EngineContext::Quit() {
	window_->Quit();
}

} //namespace Context