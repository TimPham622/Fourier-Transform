#include "Complex.h"

Complex::Complex(float r, float i) : re(r), im(i) {}

Complex Complex::operator+(const Complex& other) const {
    return Complex(re + other.re, im + other.im);
}

Complex Complex::operator-(const Complex& other) const {
    return Complex(re - other.re, im - other.im);
}

Complex Complex::operator*(const Complex& other) const {
    return Complex(
        re * other.re - im * other.im,
        re * other.im + im * other.re
    );
}

float Complex::mag() const {
    return std::sqrt(re * re + im * im);
}

float Complex::phase() const {
    return std::atan2(im, re);
}