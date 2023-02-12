# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(nextWeek)

include_directories(3rd hit material scene util vec3)

add_executable(main session5_bvh.cc color.h rtw_stb_image.h rtweekend.h )