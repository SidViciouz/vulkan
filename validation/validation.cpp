#include "validation.h"
#include <vector>
#include <cstring>
#include <iostream>

bool validation::areSupported(){
	uint32_t cnt = 0;
	vkEnumerateInstanceLayerProperties(&cnt,nullptr);
	std::vector<VkLayerProperties> availableValidationLayers(cnt);
	vkEnumerateInstanceLayerProperties(&cnt,availableValidationLayers.data());

	for(const auto& name : validationLayers){
		bool found = false;
		for(const auto& layer : availableValidationLayers){
			if(strcmp(name,layer.layerName) == 0){
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}
	return true;
}
VkResult validation::createDebugUtilsMessenger(const VkInstance instance,const VkDebugUtilsMessengerCreateInfoEXT& Info){
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");
	if(func != nullptr)
		return func(instance,&Info,nullptr,&debugUtilsMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;	
}
void validation::fillInCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& Info){
	Info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	Info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	Info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	Info.pfnUserCallback = debugCallback;
	Info.pUserData = nullptr;
}
void validation::setUpDebugUtilsMessenger(const VkInstance instance){
	VkDebugUtilsMessengerCreateInfoEXT Info{};
	fillInCreateInfo(Info);
	if(createDebugUtilsMessenger(instance,Info) != VK_SUCCESS){
		throw std::runtime_error("failed to setup debug messenger.");
	}
}
void validation::destroyDebugUtilsMessenger(const VkInstance instance) noexcept{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
	if(func != nullptr && debugUtilsMessenger != VK_NULL_HANDLE){
		func(instance,debugUtilsMessenger,nullptr);
		debugUtilsMessenger = VK_NULL_HANDLE;
	}	
}
VKAPI_ATTR VkBool32 VKAPI_CALL validation::debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,const VkDebugUtilsMessageTypeFlagsEXT messageType,const VkDebugUtilsMessengerCallbackDataEXT* const callbackData,void* const userData) noexcept{
	std::cerr << "validation layer: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}
