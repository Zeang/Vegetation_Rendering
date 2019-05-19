#pragma once

#include <glfw3.h>
#include <string>
#include <iostream>

class Window
{
public:
	GLFWwindow* window;
	int Height;
	int Width;
	std::string window_name;

	Window(std::string name, int W, int H);


};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);


