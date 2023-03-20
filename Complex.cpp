
#include "Complex.h"

constexpr inline Complex complexSquare(const Complex& in) noexcept{
    Complex out;
    out.real = in.real * in.real - in.img * in.img;
    out.img = 2.0 * in.real * in.img;
    return out;
}

inline double complex_sqr_mag(const Complex& in) noexcept{
    return in.real*in.real + in.img*in.img;
}

constexpr inline Complex operator+(const Complex& a, const Complex& b){
    Complex out;
    out.real = a.real + b.real;
    out.img = a.img + b.img;
    return out;
}