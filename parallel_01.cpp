// MandelbrotScreenSaver.cpp : Defines the entry point for the application.
//
#include <omp.h>
#include <SDL.h>
#include <cmath>
#include <list>

#include "Complex.h"
#include "Complex.cpp"

constexpr int SCREEN_WIDTH	=	640;
constexpr int SCREEN_HEIGHT =	480;
int MAX_ITER = 200;

using namespace std;
SDL_Renderer* renderer;


const int number_process = omp_get_num_procs() -2 >= 2 ? omp_get_num_procs() - 2 : 2;

Complex center;

uint8_t map (uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max){
    return (x-in_min) * (out_max -out_min) / (in_max-in_min) +out_min;
}

int main(int argc, char* argv[]){

    list<string> data;
//  SDL initial configuration
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface* screen_surface = SDL_GetWindowSurface(window);
    SDL_Surface** fractal_parts = new SDL_Surface*[number_process];

    SDL_Surface** fractal = new SDL_Surface*[1];


    fractal[0] = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

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
    while (running){
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


        drawMandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT,
                       lerp(bot_left_bound.real, center.real, progress),
                       lerp(bot_left_bound.img, center.img, progress),
                       lerp( top_right_bound.real, center.real, progress),
                       lerp(top_right_bound.img, center.img, progress), fractal);

        SDL_Rect rect;
        rect.y = 0;
        rect.w = fractal[0]->w;
        rect.h = fractal[0]->h;
        rect.x = SCREEN_WIDTH / 1 * 0;
        SDL_BlitSurface(fractal[0], NULL, screen_surface, &rect);
        SDL_FillRect(fractal[0], NULL, SDL_MapRGB(fractal[0]->format, 0, 0, 0));

        Uint64 end = SDL_GetPerformanceCounter();
        float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
        string temp_data = to_string(1.0/elapsed) + "," + to_string(elapsed) + "," + to_string(progress);
        data.push_back(temp_data);

        SDL_UpdateWindowSurface(window);

    }


    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(screen_surface);

    SDL_FreeSurface(fractal[0]);
    delete[]fractal_parts;
    delete[]fractal;
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

//  iterators
    int i, j, k;

    Uint32* target_pixel;


//  TODO this could be parallel
# pragma omp parallel private(i,j,k,c, z, target_pixel) num_threads(number_process)
# pragma omp for schedule(runtime)      
    for (i = 0; i < w; i++)
        for (j = 0; j < h; j++)
        {
            c.real = x_min + (double) (i) * x_step;
            c.img = y_max - (double) (j) * y_step;


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
                z = complexSquare(z) + c;

//              colors
                if (complex_sqr_mag(z) > 4.0){

                    k++;
                    target_pixel = (Uint32*)((Uint8*)target[0]->pixels + j * target[0]->pitch + i * target[0]->format->BytesPerPixel);

                    uint8_t bright = map(k, 0,MAX_ITER,0,255);
                    if ((k == MAX_ITER) || bright < 5){
                        bright = 0;
                    }

                    const uint8_t r = map(bright*bright,0,255,0,255);
                    const uint8_t g = bright;
                    const uint8_t b = map(::sqrt(bright),0, ::sqrt(255),0,255);

                    *target_pixel = SDL_MapRGB(target[0]->format, r,g,b);

                    break;
                }
            }

        }

}

