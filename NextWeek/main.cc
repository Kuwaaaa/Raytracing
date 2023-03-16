#include "shader.h"
#include "common.h"

#include "util/timer.h"
#include "CPU.h"

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
    int samples_per_pixel = 10;
    int max_depth = 10;
    color background(0.9, 0.9, 0.9);
    
    Scene scene(samples_per_pixel, background, image_width, aspect_radio, max_depth, SCENE_EXPLEAM::CORNELL_BOX);
    auto window = glfw_gladInit("raytracing", Hint, 1000, 600);

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

    GLuint texID3 = 0;
    glGenTextures(1, &texID3);

    CPU m_cpu(scene);
    double total_time = 0;
    int total_ssp = 0;
    vector<double> timeLog;
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
            m_cpu.render(samples_per_pixel);
            m_cpu.setGlTex(texID3);
            glBindTexture(GL_TEXTURE_2D, texID3);
            timeLog.push_back(timer.timePass());
            total_ssp  += samples_per_pixel;
            total_time += timer.timePass();
            if (timeLog.size() > 4)
                timeLog.erase(timeLog.begin());
        }
        if (ImGui::Button("Reset image"))
        {
            total_ssp = 0;
            total_time = 0;
            timeLog.clear();
            m_cpu.reset_Image();
        }
        if (ImGui::Button("Add sphere"))
        {
        }
        ImGui::InputInt("ssp", &samples_per_pixel);
        ImGui::InputInt("maxDepth", &max_depth);
        m_cpu.setMaxDepth(max_depth);

        ImGui::Spacing();
        for (const auto& i : timeLog)
        {
            ImGui::Text(("Last frame cast " + to_string(i) + " ms").c_str());
        }
        if (timeLog.size() >= 4)
        {
            ImGui::Spacing();
            ImGui::Text("...");
        }
        ImGui::Text(("Total time: " + to_string(total_time) + " ms").c_str());
        ImGui::Text(("Total ssp: "  + to_string(total_ssp)).c_str());
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
