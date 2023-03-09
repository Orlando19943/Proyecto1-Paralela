// MandelbrotScreenSaver.cpp : Defines the entry point for the application.
//
#include <SDL.h>
//#include <alloca.h>
#include "MandelbrotScreenSaver.h"

constexpr int SCREEN_WIDTH	=	680;
constexpr int SCREEN_HEIGHT =	680;
int MAX_ITER = 100;

using namespace std;
SDL_Renderer* renderer;



int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);

	complex center;
	center.real = -0.995;
	center.img = 0.3;

	complex bot_left_bound;
	bot_left_bound.real = -2;
	bot_left_bound.img = -2;
	complex top_right_bound;
	top_right_bound.real = 2;
	top_right_bound.img = 2;


	double progress = 0;
	const double base_step = 0.00005;
	const double original_step = 0.17;

	double step = original_step;
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

		


		Uint64 start = SDL_GetPerformanceCounter();

		step = step * 0.85 + base_step;
		progress += step;
		if (progress > 0.99999999)
		{
			progress = 0;
			step = original_step;
		}
		
		drawMandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT,
			lerp(bot_left_bound.real, center.real, progress),
			 lerp(bot_left_bound.img, center.img, progress),
			 lerp(center.real, top_right_bound.real,1- progress),
			 lerp(center.img,  top_right_bound.img,	1- progress));

		Uint64 end = SDL_GetPerformanceCounter();
		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
		cout << "FPS:\t" << 1.0f / elapsed << "\tDelta Time:\t" << elapsed << "\tProgress:\t" << progress << endl;

		SDL_RenderPresent(renderer);
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

inline double complex_sqr_mag(const complex& in) noexcept
{
	return sqrt(in.real*in.real + in.img*in.img);
}

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max)
{
	complex c;
	complex z;
	bool inside;
	
	const double x_step = (x_max - x_min) / (double)w;
	const double y_step = (y_max - y_min) / (double)h;
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
		{
			inside = true;
			c.real = x_min	+	(double)i * x_step;
			c.img = y_max	-	(double)j * y_step;

			z.real = 0;
			z.img = 0;

			for (int it = 0; it < MAX_ITER; it++)
			{
				z = complexSquare(z) + c;

				if (complex_sqr_mag(z) > 4.0)
				{
					it++;
					SDL_SetRenderDrawColor(renderer, 255 / it, 255 / ((i+1)%it +1), 255 / ((j+1)%it +1), 255);
					SDL_RenderDrawPoint(renderer, i, j);
					inside = false;
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