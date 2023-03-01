cmake_minimum_required (VERSION 3.1)

project (OneWeek)

include_directories(${PROJECT_SOURCE_DIR})

source_group("vec3" FILES vec3.h ray.h)
source_group("hit" FILES ./hit/hittable_list.h ./hit/hittable.h ./hit/sphere.h)
source_group("material" FILES ./material.h)

add_executable(oneweek synthesis.cc camera.h rtweekend.h)
