#include "engine_context.h"

#include "window/window_sdl.h"
#include <thread>

namespace Context {

EngineContext::EngineContext() {
	using namespace Platform;

	window_ = std::make_unique<AppWindow>();
	if (!window_) {
		LogInfoDetaill("Context Window Create Error");
	}
	auto id = std::this_thread::get_id;
	
	LogInfo("[Context][Engine]: Current Thread Id: {}",std::hash<std::thread::id>{}(std::this_thread::get_id()));
	//Open All Thread
	Init();
}

void EngineContext::Init() {
	using namespace Renderer;
	renderer_ = std::make_unique<RendererContext>(std::ref(*window_),std::ref(fps_controller_));

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