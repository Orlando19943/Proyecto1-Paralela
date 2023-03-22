// MandelbrotScreenSaver.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#define LOOP_ONCE 1
// TODO: Reference additional headers your program requires here.


struct complex 
{
	double real	= 0;
	double img	= 0;
};

constexpr inline complex complexSquare(const complex & a) noexcept;

inline double complex_sqr_mag(const complex& in) noexcept;

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target);
//void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target, complex*, complex*, int*, bool*);
//void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target, complex*, complex*, int*);



inline constexpr uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max) noexcept {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

constexpr inline complex operator+(complex a, complex b)
{
	complex out;
	out.real = a.real + b.real;
	out.img = a.img + b.img;
	return out;
}
