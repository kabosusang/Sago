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

using clock = std::chrono::steady_clock;
void EngineContext::Tick() {
	fps_controller_.StartFrame();
	//Main Thread 👇

	fps_controller_.EndFrame();
	
	//std::cout << fps_controller_.GetAverageFPS() << std::endl;
}

EngineContext::~EngineContext() {
	fps_controller_.RequestStop();
	window_.reset();
}

void EngineContext::Quit() {
	window_->Quit();
}

} //namespace Context