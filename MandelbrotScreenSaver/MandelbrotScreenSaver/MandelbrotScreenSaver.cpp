// MandelbrotScreenSaver.cpp : Defines the entry point for the application.
//
#include <SDL.h>
#include "MandelbrotScreenSaver.h"

using namespace std;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	bool running = true;
	while (running)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		switch (event.type)
		{
			case SDL_QUIT:
				running = false;
				break;
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawPoint(renderer, 100, 100);

		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
