#include "engine_context.h"

#include "event/renderer_event.h"
#include "window/window_sdl.h"
#include <functional>

namespace Context {

EngineContext::EngineContext() {
	using namespace Platform;

	window_ = std::make_unique<AppWindow>();
	if (!window_) {
		LogInfoDetail("Context Window Create Error");
	}
	auto id = std::this_thread::get_id;
	//Open All Thread
	Init();
}

void EngineContext::InitImpl() {
	using namespace Renderer;
	renderer_ = std::make_unique<RendererContext>(std::ref(*window_), std::ref(fps_controller_));
	renderer_->PutEvent(Event::RendererEventType::kRendererFrame);
	renderer_->PutEvent(
			[&]() {
				LogInfo("Event_01 is running");
			});
	int i = 10;
	renderer_->PutEvent(
			[=]() {
				LogInfo("Event_02 is running{}",i);
			});
}

void EngineContext::Tick() {
	fps_controller_.StartFrame();
	//Main Thread 👇
	renderer_->RequestFrame();



	
	fps_controller_.EndFrame();
	//fps_controller_.GetCurrentFPS_Log();
}

EngineContext::~EngineContext() {
	fps_controller_.RequestStop();
	window_.reset();
}

void EngineContext::Quit() {
	window_->Quit();
}

} //namespace Context