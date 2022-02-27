#include "renderer.h"
#include "validation/validation.h"
#include <iostream>
#include <vector>
#include <cstring>

renderer::renderer(GLFWwindow* windowHandle){
	initializeInstance();
	if constexpr (validation::areEnabled()){
		validation::setUpDebugUtilsMessenger(instanceHandle);
	}
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

	const auto extensions = getInstanceExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());	
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
	
	if constexpr (validation::areEnabled()){
		validation::fillInCreateInfo(debugUtilsMessengerCreateInfo);
		createInfo.enabledLayerCount = static_cast<uint32_t>(validation::getValidationLayers().size());
		createInfo.ppEnabledLayerNames = validation::getValidationLayers().data();
		createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugUtilsMessengerCreateInfo);
	}
	else{	
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.pNext = nullptr;
	}

	if(vkCreateInstance(&createInfo,nullptr,&instanceHandle) != VK_SUCCESS){
		throw std::runtime_error("failed to create instance.");
	}
}
void renderer::initializeSurface(){
	
}
void renderer::initializeDevice(){
}
void renderer::initializeCommandPool(){
}

//utils
std::vector<const char*> renderer::getInstanceExtensions() const{
	uint32_t cnt = 0;
	const char** glfwExtensions;
	
	glfwExtensions = glfwGetRequiredInstanceExtensions(&cnt);

	std::vector<const char*> extensions;

	for(int i=0; i<cnt; ++i)
		extensions.push_back(glfwExtensions[i]);	

	if constexpr (validation::areEnabled())
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	cnt = 0;
	vkEnumerateInstanceExtensionProperties(nullptr,&cnt,nullptr);
	std::vector<VkExtensionProperties> availables(cnt);
	vkEnumerateInstanceExtensionProperties(nullptr,&cnt,availables.data());

	for(const auto extension : extensions){
		bool found = true;
		for(const auto available : availables){
			if(strcmp(extension,available.extensionName) == 0){
				found = true;
				break;
			}
		}
		if(!found){
			using namespace std::literals::string_literals;	
			throw std::runtime_error("vulkan extension"s+extension+"is required but no supported"s);
		}
	}
			
	return extensions;	
}


