#include <iostream>
#include <iomanip>
#include <math.h>
#include "rtweekend.h"

inline double pdf(double x) {
    return 0.5*x;
}

int main() {
    int N = 1000000;
    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        auto x = sqrt(random_double(0,2));
        sum += x*x;
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum/N << '\n';
}