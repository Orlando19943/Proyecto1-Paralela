﻿#include <omp.h>
#include <SDL.h>
#include "MandelbrotScreenSaver.h"
#include <fstream>
#include <string>
#include <vector>

constexpr int SCREEN_WIDTH = 680;
constexpr int SCREEN_HEIGHT = 680;


using namespace std;
SDL_Renderer* renderer;


const int nprocs = omp_get_num_procs() - 2 >= 2 ? omp_get_num_procs() - 2 : 2;

complex center;

int main(int argc, char* argv[])
{
	if (argc > 1 && argv[1] == help_flag)
	{
		cout << help_prompt << endl;
		return 0;
	}
	vector<string> data;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

	SDL_Surface** fractal_parts = new SDL_Surface * [nprocs];

	//Puede dejar pixeles muertos
	for (int i = 0; i < nprocs; i++)
		fractal_parts[i] = SDL_CreateRGBSurface(0, SCREEN_WIDTH / nprocs, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

	//Punto objetivo para el zoom
	center.real = -1.188;
	center.img = 0.305;

	complex bot_left_bound;
	bot_left_bound.real = -2;
	bot_left_bound.img = -2;
	complex top_right_bound;
	top_right_bound.real = 2;
	top_right_bound.img = 2;

	//Parámetros de control de interpolación que implementa el zoom
	double progress = 0;
	double min_step = 0.000002;
	double original_step = 0.241;
	double step_falloff = 0.80001;

	vector<string> args;
	for (int i = 1; i < argc; i++)
		args.push_back(argv[i]);
	try {
		for (int i = 0; i < args.size(); i++)
		{
			if (args[i] == real_flag)
			{
				if (i + 1 >= args.size()) throw argv[i + 1];
				double new_creal = stod(args[i + 1], 0);
				if (new_creal > -2 && new_creal < 2) center.real = new_creal;
			}
			if (args[i] == img_flag)
			{
				if (i + 1 >= args.size()) throw argv[i + 1];
				double new_cimg = stod(args[i + 1], 0);
				if (new_cimg > -2 && new_cimg < 2) center.img = new_cimg;
			}
			if (args[i] == minstep_flag)
			{
				if (i + 1 >= args.size()) throw argv[i + 1];
				double new_minstep = stod(args[i + 1], 0);
				if (new_minstep > 1) min_step = new_minstep;
			}
			if (args[i] == decay_flag)
			{
				if (i + 1 >= args.size()) throw argv[i + 1];
				double new_decay = stod(args[i + 1], 0);
				if (new_decay > 1) step_falloff = new_decay;
			}
			if (args[i] == initstep_flag)
			{
				if (i + 1 >= args.size()) throw argv[i + 1];
				double new_initstep = stod(args[i + 1], 0);
				if (new_initstep > 1) original_step = new_initstep;
			}
			if (args[i] == maxiter_flag)
			{
				if (i + 1 >= args.size()) throw argv[i + 1];
				int new_maxiter = stoi(args[i + 1], 0);
				MAX_ITER = new_maxiter;
			}
		}
	}
	catch (char* faultyArg)
	{
		cerr << "Error in flag: " << faultyArg << endl;
		cerr << help_prompt << endl;
		return -1;
	}
	catch (std::invalid_argument e)
	{
		cerr << "Error! Check the usage with -h\n";
		cerr << help_prompt << endl;
		return -1;
	}


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

		step = step * step_falloff + min_step;
		progress += step;
		if (progress > 0.9999999999999)
		{
			progress = 0;
			step = original_step;
#if LOOP_ONCE
			running = false;
#endif
		}




#pragma omp parallel num_threads (nprocs)
		{
			drawMandelbrot(SCREEN_WIDTH / nprocs, SCREEN_HEIGHT,
				lerp(bot_left_bound.real, center.real, progress),
				lerp(bot_left_bound.img, center.img, progress),
				lerp(top_right_bound.real, center.real, progress),
				lerp(top_right_bound.img, center.img, progress), fractal_parts);
		}
		SDL_Rect rect;
		rect.y = 0;
		for (int i = 0; i < nprocs; i++)
		{
			rect.w = fractal_parts[i]->w;
			rect.h = fractal_parts[i]->h;
			rect.x = SCREEN_WIDTH / nprocs * i;

			//Copiar las partes a pantalla
			SDL_BlitSurface(fractal_parts[i], NULL, screen_surface, &rect);

			//Inicializar el siguiente frame con fondo negro
			SDL_FillRect(fractal_parts[i], NULL, SDL_MapRGB(fractal_parts[i]->format, 0, 0, 0));
		}

		Uint64 end = SDL_GetPerformanceCounter();

		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
		string temp_data = to_string(1.0 / elapsed) + "," + to_string(elapsed) + "," + to_string(progress);
		SDL_Log("FPS:\t%f \tDelta Time:\t %f \tProgress:\t %f", 1.0f / elapsed, elapsed, progress);
		data.push_back(temp_data);

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

	std::ofstream file("../../../../../results_par_func.csv", std::ios::out);
	for (auto& line : data)
		file << line << "\n";
	file.close();
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
	return in.real * in.real + in.img * in.img;
}

#define SHOW_TARGET_ZOOM 0

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target)
{
	complex c;
	complex z;
	const double x_step = (x_max - x_min) / (double)SCREEN_WIDTH;
	const double y_step = (y_max - y_min) / (double)SCREEN_HEIGHT;
	const int t_id = omp_get_thread_num();
	int x = t_id * SCREEN_WIDTH / nprocs;
	int y = 0;

	Uint32* target_pixel;

	SDL_LockSurface(target[t_id]);
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
		{
			c.real = x_min + (double)(i + x) * x_step;
			c.img = y_max - (double)j * y_step;
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

			for (int it = 0; it < MAX_ITER; it++)
			{
				//zn+1 = (z^2) + c
				z = complexSquare(z) + c;

				//Terminar la computación del punto y dibujar
				if (complex_sqr_mag(z) > 4.0)
				{
					target_pixel = (Uint32*)((Uint8*)target[t_id]->pixels
						+ j * target[t_id]->pitch
						+ i * target[t_id]->format->BytesPerPixel);

					uint8_t bright = map(it, 0, MAX_ITER, 0, 255);

					if ((it == MAX_ITER) || bright < 5) {
						bright = 0;
					}

					const uint8_t r = map(bright * bright, 0, 255, 0, 255);
					const uint8_t g = bright;
					const uint8_t b = map(::sqrt(bright), 0, ::sqrt(255), 0, 255);

					*target_pixel = SDL_MapRGB(target[0]->format, r, g, b);

					break;
				}
			}

		}

	SDL_UnlockSurface(target[t_id]);
}



