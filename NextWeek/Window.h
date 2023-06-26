#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <functional>
#include <iostream>


class Window
{
public:
	Window(const std::string& windowName, int width, int height);
	// initialize glad imgui glfw and its callback
	void initialize();
	GLFWwindow* getWindow();

private:
	std::string m_windowName;
	int m_width;
	int m_height;
	GLFWwindow* window;
};

