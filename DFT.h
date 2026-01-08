#ifndef DFT_H
#define DFT_H

#include <vector>
#include "Complex.h"

struct FourierCoef {
    float re;
    float im;
    float freq;
    float amp;  
    float phase; 
};

std::vector<FourierCoef> dft(const std::vector<Complex>& x);

#endif