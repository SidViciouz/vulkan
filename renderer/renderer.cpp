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
	
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "my universe";
	appInfo.applicationVersion = VK_MAKE_VERSION(0,0,1);
	appInfo.pEngineName = "my universe engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0,0,1);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
}
void renderer::initializeSurface(){
}
void renderer::initializeDevice(){
}
void renderer::initializeCommandPool(){
}



