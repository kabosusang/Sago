#ifndef SG_WINDOW_SDL_H
#define SG_WINDOW_SDL_H

#include <SDL3/SDL.h>
#include <memory>

#include "External/AsyncLog.h"

namespace platform{

struct SDLWindowDelter{
void operator()(SDL_Window* window){
	if (window){
		SDL_DestroyWindow(window);
		LogInfo("SDL Window Destory");
	}
}
};
using SDLWindow = std::unique_ptr<SDL_Window,SDLWindowDelter>;

//Main Thread Controll
class AppWindow : public Tools::Singleton<AppWindow>{
friend class Tools::Singleton<AppWindow>;

	AppWindow();
	~AppWindow();

private:
	SDLWindow window_;
	int width_{800};
	int height_{700};
	bool shouldexit_ = false;
public:
	inline bool ShouldExit() {return shouldexit_;}
};
} //namespace platform

#endif