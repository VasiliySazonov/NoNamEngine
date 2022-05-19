#include "window.hpp"

GLFWwindow *Window::window;
int Window::height = 0;
int Window::width = 0;

int Window::Initialize(int width, int height, std::string name)
{
	Window::width = width;
	Window::height = height;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//glfwWindowHint(GLFW_SAMPLES, 8);

	glViewport(0, 0, Window::width, Window::height);

	Window::window = glfwCreateWindow(width, height, name.c_str(), glfwGetPrimaryMonitor(), nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window::window);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	return 0;
}

int Window::ShouldClose()
{
	return glfwWindowShouldClose(Window::window);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(Window::window);
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::Terminate()
{
	glfwTerminate();
}

void Window::SetShouldClose(bool flag)
{
	glfwSetWindowShouldClose(Window::window, flag);
}

void Window::SetCursorMode(int mode)
{
	glfwSetInputMode(window, GLFW_CURSOR, mode);
}
