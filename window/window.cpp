#include "window.h"

window::window(){
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
	windowHandle = glfwCreateWindow(640,480,"my universe",NULL,NULL);
}

window::~window(){
	glfwDestroyWindow(this->windowHandle);
}

void window::loop(){
	while(!glfwWindowShouldClose(this->windowHandle)){
		glfwPollEvents();
	}
}

GLFWwindow* window::getWindowHandle(){
	return this->windowHandle;
}
