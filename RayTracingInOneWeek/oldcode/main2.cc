#include <iostream>
#include "vec3/vec3.h"
#include "vec3/ray.h"
#include "color.h"

// !!!!!!!!!!!!!!!!
// 有个问题.
// 捏麻这个ray的表达式用起来竟然和直线方程一样的
// 判断和球相交时系数一代就完事了...

// 将图片归一化到视窗大小
// 视窗是一个平截头体，有焦距和长宽.摄像机从原点(0, 0, 0)看向视窗平面。本文件中视窗平面大小为(2, 2), 焦距为1.
// 原点投影在视窗平面上为(0, 0, 1).然后左下角点为(-1, -1, 1).
// 本文件中的ray从原点射向平截头体上. 通过以左下角点为起点，以依次扫过的扫描点为终点的向量与原点相减得到ray向量.
// 扫描点通过以uv*平面宽高得到.
// 其中uv坐标是坐标范围为[0,1]的值，表示点在平面的水平、垂直方向的坐标.
// 通过ray_color函数以ray的y方向长度做线性插值，最后得到一个渐变的效果.

// 关于ray:
// ray的表达式: 
//      P = (point3)ori + (vec3)dir * t
// 书面上这么写:
//      P = A + t*b
// 跟直线的表达式是不同的.
// 直线表达式表示了一条满足了条件的无限长的直线.
// ray表示了一条以ori为中心,朝dir向量方向(不一定归一化)移动t次dir模长的有限长的线段.


// 渲染球体:
// 计算ray与球体中心的距离与球体半径的关系.
// 大于的话就判定为相离,ray未击中球体，渲染背景.
// 等于判定为相切,ray击中球体,渲染背景(可能也可以渲染球,这应该是个策略问题,实际影响可能也不大).
// 小于判定为相交,ray击中球体,渲染球体.
// ray与球体中心距离的表达式为:
//      dis^2 = (x - Cx)^2 + (y - Cy)^2 + (z - Cz)^2
// 写成向量形式为:
//      dis^2 = (P - C)^2
// 其中, P为ray表达式(参见ray条目)，C为球体中心.
// 展开为:
//      dis^2 = t^2*b·b + 2tb·(A - C) + (A - C) · (A - c)  【ex1】
// 当与球体相离时有:
//      r^2   < t^2*b·b + 2tb·(A - C) + (A - C) · (A - c)  【ex2】
// 相切时有:
//      r^2   = t^2*b·b + 2tb·(A - C) + (A - C) · (A - c)  【ex3】
// 相交时有:
//      r^2   > t^2*b·b + 2tb·(A - C) + (A - C) · (A - c)  【ex4】
// 本文件渲染球体时只考虑是否ray是否与球体相交,即判断【ex3】是否有根.
// 即判断 b^2 - 4ac > 0.


color color1(1.0, 1.0, 1.0);
color color2(0.5, 0.7, 1.0);

double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto half_b = dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = half_b*half_b - a*c;

    if (discriminant < 0){
        return -1.0;
    }
    else{
        return (-half_b - sqrt(discriminant)) / a;
    }
}

color ray_color(const ray& r)
{   
    // 画球
    // 这里球的中心z坐标取1或-1都会得到同样的结果.因为所有涉及球体中心的运算最后都平方了,就都是正数,球体中心和原点相减得出的向量的符号就寄了.
    // 这其实应该算是个bug.
    // 半径设为1图片会被球体颜色全部覆盖.如果是按在平面上的投影想象的话,应该平面还会被漏出来一点背景颜色.
    // 这里应该是由于平截头体的原点刚好落在球体表面上了.所以所有向平面上看的ray都打到了球体上.
    // 经过测试，0.898就完全被球体颜色覆盖了.这是一个线性过程.
    // -----------------------------------------
    // if (hit_sphere(point3(0, 0, -1), 0.5, r))
    //     return color(1, 0, 0);
    
    // 显示法向量
    // ---------
    // 通过hit_sphere求出法向量，
    auto t = hit_sphere(point3(0,0,-1), 0.5, r);
    if (t > .0){
        vec3 N = unit_vector(r.at(t) - vec3(0,0,-1));
        return 0.5*color(N.x()+1, N.y()+1, N.z()+1);
    }

    // 渲染背景
    // -------
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5*(unit_direction.y() + 1.0);
    // 线性插值，是一个渐变的效果
    return (1.0-t)*color1 + t*color2;
}

int main()
{
    // Image

    const auto aspect_radio = 16.0/9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_radio);

    // Camera

    auto viewport_height = 2.0;
    auto viewport_width = aspect_radio * viewport_height;
    auto focal_legth = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 -vec3(0, 0, focal_legth);

    // Render

    std::cout <<  "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
        for (int i = image_width-1; i >= 0; i--) {
            auto u = double(i) / (image_width-1);
            auto v = double(j) / (image_height-1);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            color pix_color = ray_color(r);
            write_color(std::cout, pix_color);
        }   
    }

    std::cerr << "\nDone.\n";
}