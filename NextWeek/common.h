#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>

#include "3rd/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "gl_camera.h"

#include <vector>
#include <iostream>
#include <string>

using std::vector;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path, bool isFilp = true, bool gammaCorrection = false);
// ���������
void Hint();
// ��ʼ��glfw
GLFWwindow* glfw_gladInit(const std::string& winName, std::function<void()> hint = Hint, int width = 800, int height = 600 );
// ����VAO VBO
void makeVO(const float* vertices, int arraySz, vector<float> index, unsigned int& VAO, unsigned int& VBO);

// ͨ��ȫ�ֱ�����
#define GLOBALVALUE \
const unsigned int SCR_WIDTH = 800;\
const unsigned int SCR_HEIGHT = 600;\

extern float deltaTime;\
extern float lastFrame;\

extern const unsigned int SCR_WIDTH_COMM;\
extern const unsigned int SCR_HEIGHT_COMM;\
extern const GLfloat cubeTexVertices[288];\

extern float lastX_com;\
extern float lastY_com;\
extern bool firstMouse_com;\

extern bool blinn;\
extern bool blinnKeyPressed;\

extern bool gammaEnabled;\
extern bool gammaKeyPressed;\

//extern Camera camera;





