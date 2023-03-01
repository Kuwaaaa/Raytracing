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

    color background(0.9, 0.9, 0.9);
    
    Scene scene(samples_per_pixel, background, image_width, aspect_radio, max_depth, SCENE_EXPLEAM::CORNELL_BOX);

    auto window = glfw_gladInit("raytracing");

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

    GLuint texID = loadTexture("nilou.jpg");
    GLuint texID2 = loadTexture("kkm.jpg");
    GLuint texID3 = 0;
    glGenTextures(1, &texID3);

    CPU m_cpu(scene);
    //glBindTexture(GL_TEXTURE_2D, texID);
    double time = 0;
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
            m_cpu.render(1000);
            m_cpu.setGlTex(texID3);
            glBindTexture(GL_TEXTURE_2D, texID3);
            time = timer.timePass();
        }
        
        if (ImGui::Button("Add sphere"))
        {
        }

        ImGui::Text(("Last frame cast " + to_string(time) + " ms").c_str());

        //ImGui::Image(reinterpret_cast<void*>(texID2), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        
        ImGui::Begin("View");
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Image(reinterpret_cast<void*>(texID3), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glfwTerminate();
    
   std::cerr << "\nDone.\n";
}
