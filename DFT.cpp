#include "DFT.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

std::vector<FourierCoef> dft(const std::vector<Complex>& x) {
    std::vector<FourierCoef> X;
    int N = x.size();
    
    X.reserve(N); 

    // K is the frequency
    for (int k = 0; k < N; k++) {
        Complex sum(0, 0);

        // Summation Formula
        for (int n = 0; n < N; n++) {
            float phi = (2 * PI * k * n) / N;
            
            // Euler's Formula: e^-ix = cos(x) - i*sin(x)
            Complex c(std::cos(phi), -std::sin(phi));
            
            sum = sum + (x[n] * c);
        }

        sum.re = sum.re / N;
        sum.im = sum.im / N;

        FourierCoef coef;
        coef.re = sum.re;
        coef.im = sum.im;
        coef.freq = (float)k;
        coef.amp = sum.mag();
        coef.phase = sum.phase();

        X.push_back(coef);
    }
    return X;
}