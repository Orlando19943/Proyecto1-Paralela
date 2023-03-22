// MandelbrotScreenSaver.cpp : Defines the entry point for the application.
//
#include <omp.h>
#include <SDL.h>
#include "MandelbrotScreenSaver.h"

constexpr int SCREEN_WIDTH	=	680;
constexpr int SCREEN_HEIGHT =	680;
int MAX_ITER = 250;

using namespace std;
SDL_Renderer* renderer;


const int nprocs = omp_get_num_procs() -2 >= 2 ? omp_get_num_procs() - 2 : 2;

complex center;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

	SDL_Surface** fractal_parts = new SDL_Surface*[nprocs];

	//Puede dejar pixeles muertos
	for (int i = 0; i < nprocs; i++)
		fractal_parts[i] = SDL_CreateRGBSurface(0, SCREEN_WIDTH/nprocs, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

	center.real = -1.188;
	center.img = 0.305;

	complex bot_left_bound;
	bot_left_bound.real = -2;
	bot_left_bound.img = -2;
	complex top_right_bound;
	top_right_bound.real = 2;
	top_right_bound.img = 2;


	double progress = 0;
	const double base_step = 0.000002;
	const double original_step = 0.241;

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
		

		


		Uint64 start = SDL_GetPerformanceCounter();

		step = step * 0.80001 + base_step;
		progress += step;
		if (progress > 0.9999999999999)
		{
			progress = 0;
			step = original_step;
		}
		



#pragma omp parallel num_threads (nprocs)
		{
		drawMandelbrot(SCREEN_WIDTH/nprocs, SCREEN_HEIGHT,
			lerp(bot_left_bound.real, center.real, progress),
			lerp(bot_left_bound.img, center.img, progress),
			lerp( top_right_bound.real, center.real, progress),
			lerp(top_right_bound.img, center.img, progress), fractal_parts);
		}
		SDL_Rect rect;
		rect.y = 0;
		for (int i = 0; i < nprocs; i++)
		{
			rect.w = fractal_parts[i]->w;
			rect.h = fractal_parts[i]->h;
			rect.x = SCREEN_WIDTH / nprocs * i;
			SDL_BlitSurface(fractal_parts[i], NULL, screen_surface, &rect);
			SDL_FillRect(fractal_parts[i], NULL, SDL_MapRGB(fractal_parts[i]->format, 0, 0, 0));
		}

		Uint64 end = SDL_GetPerformanceCounter();
		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
		cout << "FPS:\t" << 1.0f / elapsed << "\tDelta Time:\t" << elapsed << "\tProgress:\t" << progress << endl;

		SDL_UpdateWindowSurface(window);
		//SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(screen_surface);
	for (int i = 0; i < nprocs; i++)
		SDL_FreeSurface(fractal_parts[i]);
	delete[]fractal_parts;
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
	return in.real*in.real + in.img*in.img;
}

#define SHOW_TARGET_ZOOM 0

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target)
{
	
	const double x_step = (x_max - x_min) / (double)SCREEN_WIDTH;
	const double y_step = (y_max - y_min) / (double)SCREEN_HEIGHT;
	const int t_id = omp_get_thread_num();
	int x = t_id * SCREEN_WIDTH / nprocs;
	int y = 0;
	int it;


	complex* c = (complex*)_malloca(w * h / nprocs * sizeof(complex));
	complex* z = (complex*)_malloca(w * h / nprocs * sizeof(complex));

	Uint32* target_pixel;

	SDL_LockSurface(target[t_id]);
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
		{
			c[i + j].real = x_min + (double)(i+x) * x_step;
			c[i + j].img = y_max - (double)j * y_step;
#if SHOW_TARGET_ZOOM
			if (abs(c.real - center.real) < 0.001 || abs(c.img - center.img) < 0.001)
			{
				 target_pixel = (Uint32*)((Uint8*)target[t_id]->pixels
					+ j * target[t_id]->pitch
					+ i * target[t_id]->format->BytesPerPixel);
				*target_pixel = SDL_MapRGB(target[t_id]->format, 255, 255, 255);
			}
#endif


			z[i + j].real = 0;
			z[i + j].img = 0;
			
#pragma omp simd
			for (it = 0; it < MAX_ITER; it++)
			{
				z[i + j] = complexSquare(z[i + j]) + c[i + j];


				if (complex_sqr_mag(z[i + j]) > 4.0)
				{

					target_pixel = (Uint32*)((Uint8*)target[t_id]->pixels
						+ j * target[t_id]->pitch
						+ i * target[t_id]->format->BytesPerPixel);

					const uint8_t r = (/*(int)z.real * (int)z.img**/ it) % 235 + 20;
					const uint8_t g = (/*(int)z.real**/it) % 235 + 20;
					const uint8_t b = ((int)z[i + j].img * it) % 235 + 20;
					*target_pixel = SDL_MapRGB(target[t_id]->format, r, g, b);
				}
			}
			
		}
	
	SDL_UnlockSurface(target[t_id]);
}


constexpr inline complex operator+(const complex& a, const complex& b) 
{
	complex out;
	out.real = a.real + b.real;
	out.img = a.img + b.img;
	return out;
}