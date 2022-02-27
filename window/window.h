#pragma once

#include <GLFW/glfw3.h>

class window{
private:
	GLFWwindow* windowHandle;
public:
	window();
	~window();
	void loop();
	GLFWwindow* getWindowHandle();

};
