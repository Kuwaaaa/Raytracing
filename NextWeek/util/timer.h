#pragma once

#include <chrono>
#include <iostream>

struct Timer
{
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<double> duration;
    std::ostream& out;

    Timer(std::ostream& os)
        : out(os)
    {
        start = std::chrono::high_resolution_clock::now();
    }

    double timePass()
    {
        duration = std::chrono::high_resolution_clock::now() - start;
        return duration.count() * 1000;
    }

    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        double ms = duration.count() * 1000.0;
        out << "Timer took " << ms << " ms\n";
    }
    
};
