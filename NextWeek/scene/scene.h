#ifndef SCENE_H
#define SCENE_H

#include "../camera.h"
#include "../hit/bvh.h"
#include <vector>

class Scene{
public:
    Scene(bvh_node w, Camera c, int spp, color bg, int imgW, float ar, int max_d) 
        : world(w), cam(c), samples_per_pixel(spp), background(bg),
        image_width(imgW), aspect_radio(ar), max_depth(max_d)
    {
        image_height = static_cast<int>(image_width / aspect_radio);
        image_size = image_width * image_height * 3;
        m_ImageBuffer = new unsigned char[image_size](0);
        m_sum_ImageBuffer = new unsigned char[image_size](0);
        m_ImageVerticalIter.resize(image_width);
        m_imageHorizontalIter.resize(image_height);

        for (int i = 0; i < image_width; i++)
            m_ImageVerticalIter[i] = i;
        for (int i = 0; i < image_height; i++)
            m_imageHorizontalIter[i] = i;
    }
        
public:
    bvh_node world;
    Camera cam;
    color background;
    int samples_per_pixel;
    int image_width;
    int image_height;
    int image_size;
    int max_depth;
    float aspect_radio;

    std::vector<std::size_t> m_ImageVerticalIter, m_imageHorizontalIter;
    unsigned char* m_ImageBuffer;
    unsigned char* m_sum_ImageBuffer;
};

#endif // !SCENE_H
