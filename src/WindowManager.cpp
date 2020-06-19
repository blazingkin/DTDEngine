
#include "WindowManager.h"
#include "GLSL.h"
#include <GLFW/glfw3.h>

#include <iostream>
WindowManager * windowManager = nullptr;

void error_callback(int error, const char *description)
{
	std::cerr << description << std::endl;
}

WindowManager * WindowManager::instance = nullptr;

WindowManager::WindowManager()
{
	if (instance)
	{
		std::cerr << "One instance of WindowManager has already been created, event callbacks of new instance will not work." << std::endl;
	}

	instance = this;
}

WindowManager::~WindowManager()
{
	if (instance == this)
	{
		instance = nullptr;
	}
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool WindowManager::init(int const width, int const height)
{
	glfwSetErrorCallback(error_callback);

	// Initialize glfw library
	if (!glfwInit())
	{
		return false;
	}

	//request the highest possible version of OGL - important for mac

    // Decide GL+GLSL versions
	#if __APPLE__
		// GL 3.2 + GLSL 150
		const char* glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
	#else
		// GL 3.0 + GLSL 130
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
	#endif

	// Create a windowed mode window and its OpenGL context.
	windowHandle = glfwCreateWindow(width, height, "Viral Tower Defense", nullptr, nullptr);
	if (! windowHandle)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(windowHandle);

	// Initialize GLAD
	if (!gladLoadGL())
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// Set vsync
	glfwSwapInterval(0);
    glfwSetErrorCallback(glfw_error_callback);
	glfwSetKeyCallback(windowHandle, key_callback);
	glfwSetMouseButtonCallback(windowHandle, mouse_callback);
	glfwSetFramebufferSizeCallback(windowHandle, resize_callback);
	glfwSetScrollCallback(windowHandle, scroll_callback);
	glfwSetCursorPosCallback(windowHandle, cursor_position_callback);

	return true;
}

void WindowManager::shutdown()
{
	glfwDestroyWindow(windowHandle);
	glfwTerminate();
}

void WindowManager::setEventCallbacks(EventCallbacks * callbacks_in)
{
	callbacks = callbacks_in;
}

GLFWwindow * WindowManager::getHandle()
{
	return windowHandle;
}

void WindowManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->keyCallback(window, key, scancode, action, mods);
	}
}

void WindowManager::mouse_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->mouseCallback(window, button, action, mods);
	}
}

void WindowManager::resize_callback(GLFWwindow * window, int in_width, int in_height)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->resizeCallback(window, in_width, in_height);
	}
}

void WindowManager::scroll_callback(GLFWwindow * window, double in_deltaX, double in_deltaY)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->scrollCallback(window, in_deltaX, in_deltaY);
	}
}

void WindowManager::cursor_position_callback(GLFWwindow * window, double in_deltaX, double in_deltaY)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->cursorPositionCallback(window, in_deltaX, in_deltaY);
	}
}
