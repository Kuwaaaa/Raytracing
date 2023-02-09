#ifndef COLOR_H
#define COLOR_H

#include "vec3/vec3.h"
#include "rtweekend.h"

#include <iostream>

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel=1){
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // writte the translated [0, 255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

void write_color(std::ostream& out, float* buf, int image_width, int image_height, int ssp){

    int size = image_width * image_height * 3;
    
    for(int i = size-3; i > 0; i -= 3)
    {
        auto r = buf[i];
        auto g = buf[i + 1];
        auto b = buf[i + 2];
        // Divide the color by the number of samples.
        auto scale = 1.0 / ssp;
        r = sqrt(scale * r);
        g = sqrt(scale * g);
        b = sqrt(scale * b);
    
        // writte the translated [0, 255] value of each color component.
        out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
    }
}

#endif