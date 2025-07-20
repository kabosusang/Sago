#include "core/io/log/log.h"
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>


#include "core/Core.h"

/**
 * @brief
 * SDL MAIN ENTRY
 */
extern "C" {

/* We will use this renderer to draw into this window every frame. */
static SDL_Window* v = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		LogErrorDetaill("SDL Init Error: {}",SDL_GetError());
        return SDL_APP_FAILURE;
	}
    SDL_Window* window = SDL_CreateWindow("Sago",800,700,0);
	
    return SDL_APP_CONTINUE;
}

//Event
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

//RunTick
SDL_AppResult SDL_AppIterate(void* appstate) {
	return SDL_APP_CONTINUE;
}

//Quit
void SDL_AppQuit(void* appstate, SDL_AppResult result) {

}


}
