//
// Created by orlan on 20/03/2023.
//

#ifndef PROJECT_01_COMPLEX_H
#define PROJECT_01_COMPLEX_H

#include <iostream>
#include <SDL.h>

struct Complex{
    double real	= 0;
    double img	= 0;
};

constexpr inline Complex complexSquare(const Complex & a) noexcept;

inline double complex_sqr_mag(const Complex& in) noexcept;

void drawMandelbrot(int w, int h, double x_min, double y_min, double x_max, double y_max, SDL_Surface** target);

constexpr inline Complex operator+(const Complex& a, const Complex& b);


#endif //PROJECT_01_COMPLEX_H
