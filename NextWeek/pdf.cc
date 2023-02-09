#include <iostream>
#include <iomanip>
#include <math.h>
#include "rtweekend.h"

using namespace std;

inline double pdf(double x) {
    return 0.5*x;
}

void outer(double counter, int N)
{
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Estimate of Pi = " << 4*counter / N << '\n';
}

// Random sample in a square area, results to Diminishing returns.
void func1()
{
    int N = 1000;
    int inside_circle = 0;
    for (int i = 0; i < N; i++) {
        auto x = random_double(-1,1);
        auto y = random_double(-1,1);
        if (x*x + y*y < 1)
            inside_circle++;
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Estimate of Pi = " << 4*double(inside_circle) / N << '\n';
}


// Split square to grid, sample to invisiable
void func2()
{
    int inside_circle_stratified = 0;
    int sqrt_N = 5;
    for(int i = 0; i < sqrt_N; i++)
        for(int j = 0; j < sqrt_N; j++)
            {
                auto x = 2*((i + random_double()) / sqrt_N) - 1;
                auto y = 2*((j + random_double()) / sqrt_N) - 1;
                
                cout << "i: " << i << " j: " << j << endl;
                cout << "sum: " << (i + random_double()) << endl;
                cout << "x: " << x << " y: " << y <<endl;
                if (x*x + y*y < 1)
                    inside_circle_stratified++;
            }

    outer(inside_circle_stratified, sqrt_N*sqrt_N);
}


// Integrate x^2 betwwer [0, 2]
void func3()
{
    int N = 1000000;
    auto sum = 0.0;

    

    for (int i = 0; i < N; i++) {
        auto x = random_double(0,2);
        sum += x*x;
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << 2 * sum/N << '\n';
}

#include <algorithm>
#include <vector>
#include <execution>

int main() {
    // func3();

    int a[] = {0,1};
std::vector<int> v;
std::for_each(std::execution::par, std::begin(a), std::end(a), [&](int i) {
    v.push_back(i*2+1); // Error: data race
});
    return 0;    
}