#include "renderer.h"
#include "validation/validation.h"
#include <iostream>

renderer::renderer(GLFWwindow* windowHandle){
	initializeInstance();
	initializeSurface();
	initializeDevice();
	initializeCommandPool();	
}
renderer::~renderer(){
}
void renderer::initializeInstance(){
	if constexpr (validation::areEnabled()){
		if(!validation::areSupported()){
			throw std::runtime_error("Validation layers were requested, but are not available");
		}
	}
		
}
void renderer::initializeSurface(){
}
void renderer::initializeDevice(){
}
void renderer::initializeCommandPool(){
}



