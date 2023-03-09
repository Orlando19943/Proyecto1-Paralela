// MandelbrotScreenSaver.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

// TODO: Reference additional headers your program requires here.


struct complex 
{
	double real	= 0;
	double img	= 0;
};

constexpr inline complex complexSquare(const complex & a) noexcept;

inline double complexMag(const complex& in) noexcept;

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max);

constexpr inline complex operator+(const complex& a, const complex& b);