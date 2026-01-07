#ifndef COMPLEX_H
#define COMPLEX_H

#include <cmath>

struct Complex {
    float re;
    float im;

    Complex(float r = 0, float i = 0);

    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(const Complex& other) const;

    float mag() const;   
    float phase() const; 
};

#endif