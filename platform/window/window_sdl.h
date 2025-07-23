#ifndef SG_WINDOW_SDL_H
#define SG_WINDOW_SDL_H

#include <SDL3/SDL.h>
#include <memory>

#include "External/AsyncLog.h"
#include "External/Interface/window_interface.h"

namespace Platform {



struct SDLWindowDelter {
	void operator()(SDL_Window* window) {
		if (window) {
			SDL_DestroyWindow(window);
			LogInfo("Window Destory Success");
		}
	}
};
using SDLWindow = std::unique_ptr<SDL_Window, SDLWindowDelter>;

//Main Thread Controll
class AppWindow : public WindowInterface<AppWindow> {
	friend class WindowInterface<AppWindow>;
public:
	AppWindow();
	~AppWindow() noexcept;

	SDL_Window* GetRawImpl() const;
protected:
	void QuitImpl() const;
protected:
	SDLWindow window_;
	short width_{ 800 };
	short height_{ 700 };
	bool shouldexit_ = false;
};

template struct WindowInterface<AppWindow>;
} //namespace Platform

#endif