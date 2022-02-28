#include "renderer.h"
#include "validation/validation.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <set>

renderer::renderer(GLFWwindow* windowHandle){
	initializeInstance();
	if constexpr (validation::areEnabled()){
		validation::setUpDebugUtilsMessenger(instanceHandle);
	}
	initializeSurface(windowHandle);
	initializeDevice();
	initializeCommandPool();	
}
renderer::~renderer(){
}
void renderer::initializeInstance(){
	if constexpr (validation::areEnabled())
		if(!validation::areSupported()){
			throw std::runtime_error("Validation layers were requested, but are not available");
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
void renderer::initializeSurface(GLFWwindow* windowHandle){
	if(glfwCreateWindowSurface(instanceHandle,windowHandle,nullptr,&surfaceHandle) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface.");
}
void renderer::initializeDevice(){
	pickPhysicalDevice();
	fillInQueueFamilyIndices();
	std::set<uint32_t> QFS = {indices.graphicsFamilyIndex.value(),indices.presentationFamilyIndex.value()};
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	float priority = 1.0f;
	for(auto QF : QFS){
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = QF;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &priority;
		queueCreateInfos.push_back(createInfo);
	}

	VkPhysicalDeviceFeatures features{};
	//features.geometryShader = VK_TRUE;
	features.tessellationShader = VK_TRUE;
	features.fillModeNonSolid = VK_TRUE;
	features.depthClamp = VK_TRUE;
	features.imageCubeArray = VK_TRUE;
	//features.shaderStorageImageMultisample = VK_TRUE;
	features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
	features.samplerAnisotropy = VK_TRUE;
	features.sampleRateShading = VK_TRUE;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &features;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if constexpr (validation::areEnabled()){
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validation::getValidationLayers().size());
		deviceCreateInfo.ppEnabledLayerNames = validation::getValidationLayers().data();
	}
	else{
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
	}
	if(vkCreateDevice(physicalDeviceHandle,&deviceCreateInfo,nullptr,&deviceHandle) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device.");

	vkGetDeviceQueue(deviceHandle,indices.graphicsFamilyIndex.value(),0,&graphicsQueueHandle);
	vkGetDeviceQueue(deviceHandle,indices.presentationFamilyIndex.value(),0,&presentationQueueHandle);
	
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


void renderer::pickPhysicalDevice(){
	uint32_t cnt = 0;
	vkEnumeratePhysicalDevices(instanceHandle,&cnt,nullptr);
	if(cnt == 0)
		throw std::runtime_error("failed to find physcial device.");
	std::vector<VkPhysicalDevice> physicalDevices(cnt);
	vkEnumeratePhysicalDevices(instanceHandle,&cnt,physicalDevices.data());
	//I got only one chip which is apple m1, so I'll just pick up this processor.
	physicalDeviceHandle = physicalDevices[0];
}
void renderer::fillInQueueFamilyIndices(){
	uint32_t cnt = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle,&cnt,nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(cnt);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle,&cnt,queueFamilies.data());

	int i = 0;
	for(auto queueFamily : queueFamilies){
		if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamilyIndex = i;
		VkBool32 present = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDeviceHandle,i,surfaceHandle,&present);
		if(present)
			indices.presentationFamilyIndex = i;
		if(indices.graphicsFamilyIndex.has_value() && indices.presentationFamilyIndex.has_value()){
			return;
		}
		++i;
	}
	throw std::runtime_error("failed to fillInQueueFamilyIndices.");
}
