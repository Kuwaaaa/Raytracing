#pragma once

#include <chrono>
#include <iostream>
#include <string>

struct Timer
{
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<double> duration{ 0 };
    std::string prompt{ "Timer:" };
    std::ostream& out;

    Timer(std::ostream& os = std::cout)
        : out(os)
    {
        start = std::chrono::high_resolution_clock::now();
    }

    Timer(const std::string& p, std::ostream& os = std::cout)
        :out(os), prompt(p) 
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
        out << prompt << " took " << ms << " ms\n";
    }
    
};
