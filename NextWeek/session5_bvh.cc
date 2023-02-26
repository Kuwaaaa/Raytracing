#include "shader.h"
#include "common.h"

#include "util/timer.h"
#include "CPU.h"
#include "ray_expleam.h"
#include "rtweekend.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "util/timer.h"
#include <iostream>

using namespace std;

int main()
{
    // Image
    float aspect_radio = static_cast<float>(16.0/9.0);
    int image_width = 400;
    int samples_per_pixel = 50;
    int max_depth = 10;
    // World
    auto R = cos(pi/4);

    // Camera
    // 发现这里相机在物体里面的话渲染会变得很慢
    // 理解为每次求交都成功了
    // 通过调整lookfrom发现较近处的物体虚化比较大
    point3 lookfrom(13, 2, 13);
    point3 lookat(0, 0, 0);
    point3 vup(0, 1, 0);
    float vfov = 40.0f;
    auto dist_to_focus = (lookfrom - lookat).length();
    // auto dist_to_focus = sqrt(13);
    float aperture = 0;
    color background(0.9, 0.9, 0.9);
    
    bvh_node world;
    switch (1)
    {
    case 1:
        world = random_scene();
        aperture = 0.1f;
        break;

    default:
    case 2:
        world = two_spheres();
        break;

    case 3:
        world = two_perlin_spheres();
        break;

    case 4:
        vfov = 20;
        world = earth();
        break;

    case 5:
            world = simple_light();
            samples_per_pixel = 400;
            lookfrom = point3(26,3,6);
            lookat = point3(0,2,0);
            vfov = 20.0;
            break;

    case 6:
            world = cornell_box();
            image_width = 640;
            aspect_radio = 1.0f;
            samples_per_pixel = 400;
            background = color(0,0,0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
            
    case 7:
            world = volumeTest();
            break;
    }

    Camera cam;
    Scene scene(world, cam, samples_per_pixel, background, image_width, aspect_radio,
            max_depth);

    auto window = glfw_gladInit("raytracing");

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

    const extern GLfloat planeVertices[48];
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    Shader shader("test.vs", "test.fs");
    GLuint VAO, VBO, EBO;
    makeVO(planeVertices, sizeof(planeVertices), { 3, 3, 2 }, VAO, VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint texID = loadTexture("nilou.jpg");
    GLuint texID2 = loadTexture("kkm.jpg");
    GLuint texID3 = 0;
    glGenBuffers(1, &texID3);

    shader.use();
    shader.setInt("texture1", 0);
    
    CPU m_cpu(scene, cam);
    glBindTexture(GL_TEXTURE_2D, texID);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello");
        if (ImGui::Button("Render"))
        {   
            Timer timer(std::cout);
            m_cpu.render(50);
            m_cpu.setGlTex(texID3);
            glBindTexture(GL_TEXTURE_2D, texID3);
        }
        
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Image(reinterpret_cast<void*>(texID2), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
    
   std::cerr << "\nDone.\n";
}
