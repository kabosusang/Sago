#include "window_sdl.h"

#include "External/AsyncLog.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"

namespace Platform {

AppWindow::AppWindow() :
		window_(nullptr) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		LogErrorDetail("[Window] SDL Init {}", SDL_GetError());
		shouldexit_ = true;
	}

	window_.reset(SDL_CreateWindow(
			"Sago", width_, height_, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE));
	if (!window_) {
		LogErrorDetail("[Window] SDL Window Create {}", SDL_GetError());
		shouldexit_ = true;
	}
	SDL_ShowWindow(window_.get());

	LogInfo("[Window][Init] AppWindow Create Success");
	//LogInfo("地址: {:#x}",reinterpret_cast<uintptr_t>(&Core::Log::AsyncLog::Instance()));
}

AppWindow::~AppWindow() noexcept {
	window_.reset();
}

void AppWindow::QuitImpl() const {
	SDL_Quit();
}

SDL_Window* AppWindow::GetRawImpl() const {
	return window_.get();
}

std::pair<int, int> AppWindow::GetWindowSizeInPixel() const{
	int width, height;
	//Thread Safe?
	SDL_GetWindowSizeInPixels(GetRawImpl(), &width, &height);
	return {width,height};
}

void AppWindow::GetWindowSizeInPixel(std::pair<int, int>& wh){
	SDL_GetWindowSizeInPixels(GetRawImpl(), &wh.first, &wh.second);
}

SDL_Window* GetWindowPtr(const AppWindow& window) {
	return window.GetRawImpl();
}

} //namespace Platform