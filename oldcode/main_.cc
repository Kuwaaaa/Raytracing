#include <iostream>
#include "vec3/vec3.h"
#include "color.h"
int main()
{
    const int img_w = 256;
    const int img_h = 256;

    std::cout << "P3\n" << img_w << ' ' << img_h << "\n255\n";

    for (int j = img_h - 1; j >= 0; --j)
        {
            std::cerr << "\rScanlines remaining: " << ' '<< std::flush;
            for (int i = 0; i < img_w; ++i)
            {
                color pix_color(double(i) / (img_w - 1), 
                                double(j) / (img_h - 1), 
                                1);
                write_color(std::cout, pix_color);      
            }
        }
    std::cerr << "\nDone.\n";
}