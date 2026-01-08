#include "FFT.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

void fft(std::vector<CArray>& x) {
    const size_t N = x.size();
    
    if (N <= 1) return;

    std::vector<CArray> even(N / 2);
    std::vector<CArray> odd(N / 2);

    for (size_t i = 0; i < N / 2; ++i) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    fft(even);
    fft(odd);

    for (size_t k = 0; k < N / 2; ++k) {
        CArray t = std::polar(1.0f, -2.0f * PI * k / N) * odd[k];
        
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}