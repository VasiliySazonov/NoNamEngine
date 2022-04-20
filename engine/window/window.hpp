#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

class Window
{
public:
	static GLFWwindow *window;
	static int width, height;
	static int Initialize(int width, int heigh, std::string name);
	static int ShouldClose();
	static void SwapBuffers();
	static void PollEvents();
	static void Terminate();
	static void SetShouldClose(bool flag);
	static void SetCursorMode(int mode);
};
