#include "../rtweekend.h"

#include "../color.h"
#include "../hit/hittable_list.h"
#include "../hit/sphere.h"
#include "../camera.h"
#include "../material/material.h"
#include "../hit/moving_sphere.h"
#include "../hit/bvh.h"
#include "../hit/texture.h"
#include "../hit/aarect.h"
#include "../hit/box.h"

class Scene{
public:
    bvh_node world;
    camera cam;
    int samples_per_pixel;
    int image_width;
    int image_height;
    color background;
    float aspect_radio;
    int max_depth;
public:
    Scene(bvh_node w, camera c, int spp, color bg, int imgW, float ar, int max_d) 
        : world(w), cam(c), samples_per_pixel(spp), background(bg),
            image_width(imgW), aspect_radio(ar), max_depth(max_d) { image_height = static_cast<int>(image_width / aspect_radio);}

};