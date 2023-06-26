#include "shader.h"
#include "common.h"
#include "CompShader.h"

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
    float aspect_radio = static_cast<float>(16.0 / 9.0);
    int image_width = 400;
    int image_height = image_width / aspect_radio;
    int samples_per_pixel = 10;
    int max_depth = 5;
    double total_time = 0;
    int total_ssp = 0;
    bool isFirst = true;    
    bool isPar = true;
    vector<double> timeLog;
    vector<double> fpsLog;

    float deltaTime = 0.f;
    float lastFrame = 0.f;
    int fCounter = 0;
    color background(0.9, 0.9, 0.5);

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

    unsigned int texture;

    CompShader cShader("./compTest.comp");

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_width, image_height, 0, GL_RGBA,
        GL_FLOAT, NULL);

    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    CPU m_cpu(scene, isPar);


    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
        work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
        work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    int work_grp_inv;

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fpsLog.push_back(1 / deltaTime);

        if (fCounter > 500) {
            fCounter = 0;
            double avgFPS = 0;
            for (auto i : fpsLog)
                avgFPS += i;
            avgFPS /= fpsLog.size();
            //std::cout << "AVGER FPS: " << avgFPS << std::endl;
        }
        else {
            fCounter++;
        }

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        cShader.use();
        cShader.setFloat("t", currentFrame);
        glDispatchCompute((unsigned int)image_width / 10, (unsigned int)image_height / 10, 1);

        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        ImGui::Begin("computer_shader");
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Image(reinterpret_cast<void*>(texture), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::Begin("Hello");
        if (ImGui::Button("Render") || isFirst)
        {
            isFirst = false;
            Timer timer(std::cout);
            m_cpu.render(samples_per_pixel);
            m_cpu.setGlTex(texID3);
            glBindTexture(GL_TEXTURE_2D, texID3);

            timeLog.insert(timeLog.begin(), timer.timePass());
            total_ssp += samples_per_pixel;
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

            Timer timer(std::cout);
            m_cpu.render(samples_per_pixel);
            m_cpu.setGlTex(texID3);
            glBindTexture(GL_TEXTURE_2D, texID3);

            timeLog.insert(timeLog.begin(), timer.timePass());
            total_ssp += samples_per_pixel;
            total_time += timer.timePass();
            if (timeLog.size() > 4)
                timeLog.erase(timeLog.begin());
        }
        if (ImGui::Button("Add sphere"))
        {

        }
        ImGui::InputInt("ssp", &samples_per_pixel);
        ImGui::InputInt("maxDepth", &max_depth);
        m_cpu.setMaxDepth(max_depth);

        if (ImGui::Checkbox("Parallel", &isPar))
            m_cpu.setPar(isPar);

        ImGui::Spacing();
        for (const auto& i : timeLog)
            ImGui::Text(("Last frame cast " + to_string(i) + " ms").c_str());

        if (timeLog.size() >= 4)
        {
            ImGui::Spacing();
            ImGui::Text("...");
        }
        ImGui::Text(("Total time: " + to_string(total_time) + " ms").c_str());
        ImGui::Text(("Total ssp: " + to_string(total_ssp)).c_str());
        ImGui::End();

        ImGui::Begin("View");
        viewportSize = ImGui::GetContentRegionAvail();
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
