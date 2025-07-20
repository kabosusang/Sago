#include "window_sdl.h"
#include "core/io/log/log.h"

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
}

AppWindow::~AppWindow() {
	window_.reset();
}

} //namespace platform