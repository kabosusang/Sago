#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#include <SDL3/SDL_main.h>

#include "runtime.h"

/**
 * @brief
 * SDL MAIN ENTRY
 */
extern "C" {

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	auto& runtime = Runtime::Instance();
	if (!runtime.Init()){
		return SDL_APP_FAILURE;
	}
    return SDL_APP_CONTINUE;
}

//Event
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	auto& runtime = Runtime::Instance();
	
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}

	if (event->type == SDL_EVENT_KEY_DOWN){
		runtime.Pause();
	}
	return SDL_APP_CONTINUE;
}

//RunTick
SDL_AppResult SDL_AppIterate(void* appstate) {
	auto& runtime = Runtime::Instance();
	runtime.Tick();
	if (!runtime.ShouldExit()){
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

//Quit
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	auto& runtime = Runtime::Instance();
	runtime.Quit();
}


}
