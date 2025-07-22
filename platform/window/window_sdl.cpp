#include "window_sdl.h"

#include "External/AsyncLog.h"


namespace platform {

AppWindow::AppWindow() :
		window_(nullptr) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		LogErrorDetaill("SDL Init Error: {}", SDL_GetError());
		shouldexit_ = true;
	}

	window_.reset(SDL_CreateWindow(
			"Sago", width_, height_, 0));
	if (!window_) {
		LogErrorDetaill("Window Create Error: {}", SDL_GetError());
		shouldexit_ = true;
	}

	LogInfo("Window Init Success{}",1);
	LogInfo("地址: {:#x}",reinterpret_cast<uintptr_t>(&Core::Log::AsyncLog::Instance()));

}

AppWindow::~AppWindow() {
	window_.reset();
}

} //namespace platform