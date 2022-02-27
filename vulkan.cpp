#include "window/window.h"
#include "renderer/renderer.h"
#include <iostream>

int main(){
	
	window window;
	renderer renderer(window.getWindowHandle());	
	try{	
		window.loop();
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
