#pragma once
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <Event.h>
#include <iostream>
#include <functional>
#include <sstream>

class Window 
{
public:
	Window(const char* title, int width, int height );
	~Window();

	bool ShouldClose() { return glfwWindowShouldClose(_glfwWindowPtr); }
    void SwapBuffers() { glfwSwapBuffers(_glfwWindowPtr); }

    void BeginFrame();
    void EndFrame();

    GLFWwindow* GetRawWindow() { return _glfwWindowPtr; }
    double GetWindowTime() { return glfwGetTime(); }

    inline static int Width = 0;
    inline static int Height = 0;
private:
	GLFWwindow* _glfwWindowPtr;

    static void OnResize(GLFWwindow* window, int width, int height)
    {
        Width = width; Width = height;
        glViewport(0, 0, width, height);
    }
};