#include "SDL3/SDL.h"

#include "core/Core.h"
#include "core/io/log/log.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Sago",800,700,0);

	SDL_Event event;
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
				quit = true;
		}
		LogInfo("Runing...");
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}