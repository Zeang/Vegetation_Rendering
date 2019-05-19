#include "mywindow.h"
#include "glad.h"

const GLuint screen_width = 1920;
const GLuint screen_height = 1080;

Window::Window(std::string name, int W, int H)
{
	
	
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	//window = glfwCreateWindow(W, H, name.c_str(), glfwGetPrimaryMonitor(), nullptr); // Windowed
	window = glfwCreateWindow(W, H, name.c_str(), NULL, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwSetWindowAspectRatio(window,16, 9);
	glfwSetWindowPos(window, (screen_width - W) / 2, (screen_height - H) / 2);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return ;
	}

	// Define the viewport dimensions
	glViewport(0, 0, W, H);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_ALWAYS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
}
