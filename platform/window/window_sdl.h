#ifndef SG_WINDOW_SDL_H
#define SG_WINDOW_SDL_H

#include "SDL3/SDL_log.h"
#include <SDL3/SDL.h>

class SDLWindow {
    public:
	static int Init() {

		if (!SDL_Init(SDL_INIT_VIDEO)) {
			SDL_Log("SDL_Init failed: %s", SDL_GetError());
			return -1;
		}
	}
};

#endif