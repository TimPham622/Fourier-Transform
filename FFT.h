#ifndef FFT_H
#define FFT_H

#include <vector>
#include <complex>

typedef std::complex<float> CArray; 

void fft(std::vector<CArray>& x);

#endif