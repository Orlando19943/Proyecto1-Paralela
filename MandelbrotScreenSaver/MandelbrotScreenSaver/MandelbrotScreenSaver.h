// MandelbrotScreenSaver.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <iostream>
#include <string>

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

std::string real_flag = "--real";
std::string img_flag = "--img";
std::string minstep_flag = "--minstep";
std::string decay_flag = "--decay";
std::string initstep_flag = "--initstep";
std::string maxiter_flag = "--maxiter";


std::string help_flag = "-h";
std::string help_prompt = "Usage:\
\n--real	<Real coordinate to zoom to. Ignored if value is outside of (-2, 2)>\
\n--img		<Imaginary coordinate to zoom to. (don't write i) Ignored if value is outside of (-2, 2)>\
\n--minstep <Step lower limit.		Ignored if greater than 1>\
\n--decay	<Rate of step decay.	Ignored if greater than 1>\
\n--initstep <Initial zoom rate.	Ignored if greater than 1>\
\n--maxiter <Iteration cap.			Defaults to 250>";

int MAX_ITER = 250;

