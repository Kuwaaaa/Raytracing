//#ifndef COLOR_H
//#define COLOR_H
//
//#include "rtweekend.h"
//#include "vec3/vec3.h"
//#include "vec3/ray.h"
//
//#include "scene/scene.h"
//#include <iostream>
//
//void write_color(std::ostream& out, color pixel_color, int samples_per_pixel=1){
//    auto r = pixel_color.x();
//    auto g = pixel_color.y();
//    auto b = pixel_color.z();
//
//    // Divide the color by the number of samples.
//    auto scale = 1.0 / samples_per_pixel;
//    r = sqrt(scale * r);
//    g = sqrt(scale * g);
//    b = sqrt(scale * b);
//
//    // writte the translated [0, 255] value of each color component.
//    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
//        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
//        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
//}
//
//void write_color(std::ostream& out, const Scene& scene){
//
//    static int write_color_fitst = 1;
//    if (write_color_fitst--)
//    {
//         out << "P3\n" << scene.image_width << " " << scene.image_height << "\n255\n";
//    }
//
//    auto& buf = scene.m_ImageBuffer;
//    
//    for(int i = scene.image_size-3; i >= 0; i -= 3)
//    {
//        // writte the translated [0, 255] value of each color component.
//        out << static_cast<int>(buf[i]) << ' '
//            << static_cast<int>(buf[i+1]) << ' '
//            << static_cast<int>(buf[i+2]) << '\n';
//    }
//}
//
//#endif
