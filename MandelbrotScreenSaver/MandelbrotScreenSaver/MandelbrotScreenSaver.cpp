// MandelbrotScreenSaver.cpp : Defines the entry point for the application.
//
#include <SDL.h>
//#include <alloca.h>
#include "MandelbrotScreenSaver.h"

constexpr int SCREEN_WIDTH	=	680;
constexpr int SCREEN_HEIGHT =	480;
int MAX_ITER = 50;

using namespace std;
SDL_Renderer* renderer;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);

	complex center;

	bool running = true;
	while (running)
	{
		Uint64 start = SDL_GetPerformanceCounter();
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

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		drawMandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT, -2, -2, 2, 2);

		SDL_RenderPresent(renderer);
		Uint64 end = SDL_GetPerformanceCounter();
		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
		cout << "FPS:\t" << 1.0f / elapsed << "\tDelta Time:\t" << elapsed << endl;
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}


constexpr inline complex complexSquare(const complex& in) noexcept
{
	complex out;
	out.real = in.real * in.real - in.img * in.img;
	out.img = 2.0 * in.real * in.img;
	return out;
}

inline double complexMag(const complex& in) noexcept
{
	return sqrt(in.real*in.real + in.img*in.img);
}

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max)
{
	complex c;
	complex z;
	const double x_step = (x_max - x_min) / (double)w;
	const double y_step = (y_max - y_min) / (double)h;
	for (int i = 0; i < w; i ++)
		for (int j = 0; j < h; j ++)
		{
			c.real = x_min	+	(double)i * x_step;
			c.img = y_min	+	(double)j * y_step;

			z.real = 0;
			z.img = 0;

			for (int it = 0; it < MAX_ITER; it++)
			{
				z = complexSquare(z) + c;
				if (complexMag(z) > 2.0)
				{
					SDL_RenderDrawPoint(renderer, i, j);
					break;
				}
			}
		}

	
}


constexpr inline complex operator+(const complex& a, const complex& b) 
{
	complex out;
	out.real = a.real + b.real;
	out.img = a.img + b.img;
	return out;
}