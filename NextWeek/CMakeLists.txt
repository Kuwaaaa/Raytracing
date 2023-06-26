# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(nextWeek)

find_package(imgui REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(opengl REQUIRED)

add_executable(nextWeek
    aaa.cc
    camera.h
    cc.sh
    ClassDiagram.cd
    color.h
    common.cpp
    common.h
    CompShader.cpp
    CompShader.h
    compTest.comp
    CPU.cpp
    CPU.h
    Device.cpp
    Device.h
    glfw3.dll
    gl_camera.h
    imgui.ini
    kkm.jpg
    main.cc
    miniHelper.cpp
    miniHelper.h
    nilou.jpg
    nnm.jpg
    ONB.cpp
    ONB.h    
	pdf.cpp    
	pdf.h    
	raytracing.h    
	ray_expleam.cpp    
	ray_expleam.h    
	shader.h    
	stb_image.cpp    
	test.frag    
	test.jpg    
	test.vert    
	timage.ppm    
	toojpeg.cpp    
	toojpeg.h    
	Window.cpp    
	Window.h
)
	# Add subdirectories here.
# add_subdirectory(subdirectory_name)

include_directories(3rd hit material scene util vec3)
