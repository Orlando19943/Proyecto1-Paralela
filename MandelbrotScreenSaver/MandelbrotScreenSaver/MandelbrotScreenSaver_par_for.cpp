#include <omp.h>
#include <SDL.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>

#include "MandelbrotScreenSaver.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 640;
int MAX_ITER = 200;

using namespace std;
SDL_Renderer* renderer;


const int number_process = omp_get_num_procs() - 2 >= 2 ? omp_get_num_procs() - 2 : 2;

complex center;



int main(int argc, char* argv[]) {

    vector<string> data;
    //  SDL initial configuration
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Mandelbrot set screen saver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface* screen_surface = SDL_GetWindowSurface(window);


    //  first values of the of the fractal → i guess
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
    while (running) {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        }

        Uint64 start = SDL_GetPerformanceCounter();

        SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0, 0, 0));
        step = step * 0.80001 + base_step;
        progress += step;
        if (progress > 0.9999999999999)
        {
            progress = 0;
            step = original_step;
#if LOOP_ONCE
            running = false;
#endif
        }


        drawMandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT,
            lerp(bot_left_bound.real, center.real, progress),
            lerp(bot_left_bound.img, center.img, progress),
            lerp(top_right_bound.real, center.real, progress),
            lerp(top_right_bound.img, center.img, progress), &screen_surface);



        

        Uint64 end = SDL_GetPerformanceCounter();
        float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
        string temp_data = to_string(1.0 / elapsed) + "," + to_string(elapsed) + "," + to_string(progress);
        SDL_Log("FPS:\t%f \tDelta Time:\t %f \tProgress:\t %f", 1.0f / elapsed, elapsed, progress);
        data.push_back(temp_data);

        SDL_UpdateWindowSurface(window);

    }


    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(screen_surface);

    SDL_DestroyWindow(window);
    SDL_Quit();

    // write data to file
    std::ofstream file("../../../../../results_par_for.csv", std::ios::out);
    for (auto& line : data)
        file << line << "\n";
    file.close();

    return 0;
}

#define SHOW_TARGET_ZOOM 0

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target) {
    complex c;
    complex z;
    const double x_step = (x_max - x_min) / (double)SCREEN_WIDTH;
    const double y_step = (y_max - y_min) / (double)SCREEN_HEIGHT;

    //  iterators
    int i, j, k;

    Uint32* target_pixel;


    //  TODO this could be parallel
#pragma omp parallel private(i,j,k,c, z, target_pixel) num_threads(number_process)
#pragma omp for schedule(runtime)
    for (i = 0; i < w; i++)
        for (j = 0; j < h; j++)
        {
            c.real = x_min + (double)(i)*x_step;
            c.img = y_max - (double)(j)*y_step;


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

            for (k = 0; k < MAX_ITER; k++) {
                //              recursive calculation of zn
                z = complexSquare(z) + c;

                //              colors
                if (complex_sqr_mag(z) > 4.0) 
                {

                    target_pixel = (Uint32*)((Uint8*)target[0]->pixels + j * target[0]->pitch + i * target[0]->format->BytesPerPixel);

                    uint8_t bright = map(k, 0, MAX_ITER, 0, 255);

                    if ((k == MAX_ITER) || bright < 5) {
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