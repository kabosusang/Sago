#include "window_sdl.h"

#include "External/AsyncLog.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"


namespace Platform {

AppWindow::AppWindow() :
		window_(nullptr) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		LogErrorDetaill("Error: SDL Init {}", SDL_GetError());
		shouldexit_ = true;
	}

	window_.reset(SDL_CreateWindow(
			"Sago", width_, height_, SDL_WINDOW_VULKAN ));
	if (!window_) {
		LogErrorDetaill("Error: Window Create {}", SDL_GetError());
		shouldexit_ = true;
	}

	LogInfo("Window Init Success");
	//LogInfo("地址: {:#x}",reinterpret_cast<uintptr_t>(&Core::Log::AsyncLog::Instance()));
	
}

AppWindow::~AppWindow() noexcept {
	window_.reset();
}


void AppWindow::QuitImpl() const{
	SDL_Quit();
}

SDL_Window* AppWindow::GetRawImpl() const{
	return window_.get();
}




} //namespace platform