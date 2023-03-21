﻿// MandelbrotScreenSaver.cpp : Defines the entry point for the application.
//
#include <omp.h>
#include <SDL.h>
#include <cmath>

#include "Complex.h"
#include "Complex.cpp"

constexpr int SCREEN_WIDTH	=	680;
constexpr int SCREEN_HEIGHT =	680;
int MAX_ITER = 200;

using namespace std;
SDL_Renderer* renderer;


const int number_process = omp_get_num_procs() -2 >= 2 ? omp_get_num_procs() - 2 : 2;

Complex center;

uint8_t map (uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max){
    return (x-in_min) * (out_max -out_min) / (in_max-in_min) +out_min;
}

int main(int argc, char* argv[]){

//  SDL initial configuration
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);
	SDL_Surface** fractal_parts = new SDL_Surface*[number_process];


	//Puede dejar pixeles muertos
	for (int i = 0; i < number_process; i++)
		fractal_parts[i] = SDL_CreateRGBSurface(0, SCREEN_WIDTH/number_process, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

//  first values of the of the fractal → i guess
	center.real = -1.188;
	center.img = 0.305;

	Complex bot_left_bound;
	bot_left_bound.real = -2;
	bot_left_bound.img = -2;
	Complex top_right_bound;
	top_right_bound.real = 2;
	top_right_bound.img = 2;


	double progress = 0;
	const double base_step = 0.00002;
	const double original_step = 0.243;

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



#pragma omp parallel num_threads (number_process)
        {
		drawMandelbrot(SCREEN_WIDTH/number_process, SCREEN_HEIGHT,
			lerp(bot_left_bound.real, center.real, progress),
			lerp(bot_left_bound.img, center.img, progress),
			lerp( top_right_bound.real, center.real, progress),
			lerp(top_right_bound.img, center.img, progress), fractal_parts);
		}

		SDL_Rect rect;
		rect.y = 0;
		for (int i = 0; i < number_process; i++){
			rect.w = fractal_parts[i]->w;
			rect.h = fractal_parts[i]->h;
			rect.x = SCREEN_WIDTH / number_process * i;
			SDL_BlitSurface(fractal_parts[i], NULL, screen_surface, &rect);
			SDL_FillRect(fractal_parts[i], NULL, SDL_MapRGB(fractal_parts[i]->format, 0, 0, 0));
		}

		Uint64 end = SDL_GetPerformanceCounter();
		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
		cout << "FPS:\t" << 1.0f / elapsed << "\tDelta Time:\t" << elapsed << "\tProgress:\t" << progress << endl;
        SDL_Log("FPS:\t%f \tDelta Time:\t %f \tProgress:\t %f", 1.0f / elapsed , elapsed, progress);

		SDL_UpdateWindowSurface(window);
		//SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(screen_surface);
	for (int i = 0; i < number_process; i++)
		SDL_FreeSurface(fractal_parts[i]);
	delete[]fractal_parts;
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

#define SHOW_TARGET_ZOOM 0

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target){
    Complex c;
    Complex z;
    const double x_step = (x_max - x_min) / (double)SCREEN_WIDTH;
    const double y_step = (y_max - y_min) / (double)SCREEN_HEIGHT;
    const int t_id = omp_get_thread_num();
    int x = t_id * SCREEN_WIDTH / number_process;

//  iterators
    int i, j, k;

    Uint32* target_pixel;

    SDL_LockSurface(target[t_id]);

//  TODO this could be parallel
    for (i = 0; i < w; i++)
        for (j = 0; j < h; j++)
        {
            c.real = x_min + (double) (i+x) * x_step;
            c.img = y_max - (double) j * y_step;


#if SHOW_TARGET_ZOOM
            if (abs(c.real - center.real) < 0.001 || abs(c.img - center.img) < 0.001)
			{
				 target_pixel = (Uint32*)((Uint8*)target[t_id]->pixels
					+ j * target[t_id]->pitch
					+ i * target[t_id]->format->BytesPerPixel);
				*target_pixel = SDL_MapRGB(target[t_id]->format, 255, 255, 255);
			}
#endif

            z.real = 0;
            z.img = 0;

            for (k = 0; k < MAX_ITER; k++){
//              recursive calculation of zn
//              zn = zn-1^2 + c
                z = complexSquare(z) + c;

//              colors
                if (complex_sqr_mag(z) > 4.0){
                    k++;

                    target_pixel = (Uint32*)((Uint8*)target[t_id]->pixels
                                             + j * target[t_id]->pitch
                                             + i * target[t_id]->format->BytesPerPixel);

                    uint8_t bright = map(k, 0,MAX_ITER,0,255);
                    if ((k == MAX_ITER) || bright < 5){
                        bright = 0;
                    }
                    const uint8_t r = map(bright*bright,0,255,0,255);
                    const uint8_t g = bright;
                    const uint8_t b = map(::sqrt(bright),0, ::sqrt(255),0,255);

//                    const uint8_t r = (k) % 255;
//                    const uint8_t g = (k) % 255;
//                    const uint8_t b = ((int) z.img * k) % 235;
                    *target_pixel = SDL_MapRGB(target[t_id]->format, r, g, b);

                    break;
                }
            }

        }

    SDL_UnlockSurface(target[t_id]);
}

