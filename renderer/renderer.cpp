#include "renderer.h"
#include "validation/validation.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <set>
#include <functional>
#include "rendererUtility.h"

renderer::renderer(GLFWwindow* windowHandle,const renderer::config& configHandle): wHandle{windowHandle} {
	//setting context part.
	initializeInstance();
	if constexpr (validation::areEnabled()){
		validation::setUpDebugUtilsMessenger(instanceHandle);
	}
	initializeSurface(windowHandle);
	initializeDevice();
	initializeCommandPool();

	//presentation part.
	enableVsyncHandle = configHandle.enableVsync;
	enableMultisampleHandle = configHandle.enableMultisample;
	initializePipelineCache();
	initializeSynchronizationPrimitives();	
	initializePresentationObjects();	
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
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = indices.graphicsFamilyIndex.value();
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	
	if(vkCreateCommandPool(deviceHandle,&createInfo,nullptr,&commandPoolHandle) != VK_SUCCESS)
		throw std::runtime_error("fail to create commandPool.");	
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
[[nodiscard]] renderer::surfaceProperties renderer::getSurfaceProperties() const{
	surfaceProperties sp{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceHandle,surfaceHandle,&sp.surfaceCapabilities);
	
	uint32_t cnt = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle,surfaceHandle,&cnt,nullptr);
	
	if(cnt > 0){
		sp.surfaceFormats.resize(cnt);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceHandle,surfaceHandle,&cnt,sp.surfaceFormats.data());
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle,surfaceHandle,&cnt,nullptr);
	if(cnt > 0){
		sp.presentationModes.resize(cnt);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceHandle,surfaceHandle,&cnt,sp.presentationModes.data());
	}
	
	return sp;
	
}
[[nodiscard]] VkSurfaceFormatKHR renderer::getBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats) const{
	for(const auto& availableSurfaceFormat : availableSurfaceFormats){
		if(availableSurfaceFormat.format == VK_FORMAT_UNDEFINED)
			return VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
		else if(availableSurfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableSurfaceFormat;
	}
	return availableSurfaceFormats.front();	
}
[[nodiscard]] VkPresentModeKHR renderer::getBestPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes) const{
	bool immediateMode = false;
	
	for(const auto availablePresentationMode : availablePresentationModes){
		if(availablePresentationMode == VK_PRESENT_MODE_MAILBOX_KHR && enableVsyncHandle)
			return availablePresentationMode;
		else if(availablePresentationMode == VK_PRESENT_MODE_IMMEDIATE_KHR){
			if(!enableVsyncHandle)
				return availablePresentationMode;
			immediateMode = true;
		}
	}	
	if(!enableVsyncHandle && !immediateMode)
		throw std::runtime_error("failed to get non-vsync vulkan presentation mode.");
	
	return immediateMode ? VK_PRESENT_MODE_IMMEDIATE_KHR : VK_PRESENT_MODE_FIFO_KHR;
}
[[nodiscard]] VkExtent2D renderer::getDrawableSurfaceExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const{
	
	if(surfaceCapabilities.currentExtent.width < std::numeric_limits<uint32_t>::max() ||
		surfaceCapabilities.currentExtent.height < std::numeric_limits<uint32_t>::max())
		return surfaceCapabilities.currentExtent;
	else{
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(wHandle,&width,&height);
		glm::uvec2 drawableWindowSize = {static_cast<uint32_t>(width),static_cast<uint32_t>(height)};
		
		VkExtent2D actualExtent = {drawableWindowSize.x,drawableWindowSize.y};
		actualExtent.width = std::clamp(actualExtent.width,surfaceCapabilities.minImageExtent.width,surfaceCapabilities.maxImageExtent.width);		
		actualExtent.height = std::clamp(actualExtent.height,surfaceCapabilities.minImageExtent.height,surfaceCapabilities.maxImageExtent.height);
		return actualExtent;		
	}
} 
void renderer::initializePipelineCache(){
	VkPipelineCacheCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	createInfo.initialDataSize = 0;
	createInfo.pInitialData = nullptr;
	
	if(vkCreatePipelineCache(deviceHandle,&createInfo,nullptr,&pipelineCacheHandle) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline cache.");
}
void renderer::initializeSynchronizationPrimitives(){
	perFrameSynchronization.resize(2);
	
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(auto& [imageAvailableSemaphore, renderFinishedSemaphore, inFlightFence] : perFrameSynchronization){
		const std::array<std::reference_wrapper<VkSemaphore>,2> semaphores{std::ref(imageAvailableSemaphore),std::ref(renderFinishedSemaphore)};
		
		for(auto & semaphore : semaphores){
			if(vkCreateSemaphore(deviceHandle,&semaphoreCreateInfo,nullptr,&semaphore.get()) != VK_SUCCESS)
				throw std::runtime_error("failed to create semaphore.");
		}
		if(vkCreateFence(deviceHandle,&fenceCreateInfo,nullptr,&inFlightFence) != VK_SUCCESS)
			throw std::runtime_error("failed to create fence.");
	}
}
void renderer::initializePresentationObjects(){
	initializeSwapchain();
	initializeRenderPass();

	initializeFramebuffers();
	allocateCommandBuffers();	
}
void renderer::initializeSwapchain(){
	const renderer::surfaceProperties sp = getSurfaceProperties();
	
	surfaceFormatHandle = getBestSurfaceFormat(sp.surfaceFormats);
	presentationModeHandle = getBestPresentationMode(sp.presentationModes);
	swapchainExtentHandle = getDrawableSurfaceExtent(sp.surfaceCapabilities);

	uint32_t minImageCount = sp.surfaceCapabilities.minImageCount + 1;
	if(sp.surfaceCapabilities.maxImageCount > 0 && sp.surfaceCapabilities.maxImageCount < minImageCount)
		minImageCount = sp.surfaceCapabilities.maxImageCount;
	
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surfaceHandle;
	createInfo.minImageCount = minImageCount;
	createInfo.imageFormat = surfaceFormatHandle.format;
	createInfo.imageColorSpace = surfaceFormatHandle.colorSpace;
	createInfo.imageExtent = swapchainExtentHandle;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	const std::array<uint32_t,2> qfidc{indices.graphicsFamilyIndex.value(),indices.presentationFamilyIndex.value()};
	
	if(indices.graphicsFamilyIndex.value() == indices.presentationFamilyIndex.value()){
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	else{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = qfidc.data();
	}
	createInfo.preTransform = sp.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentationModeHandle;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	if(vkCreateSwapchainKHR(deviceHandle,&createInfo,nullptr,&swapchainHandle) != VK_SUCCESS)
		throw std::runtime_error("failed to create swapchain.");

	initializeSwapchainImages();
	
		
}
void renderer::initializeSwapchainImages(){
	uint32_t cnt = 0;
	
	if(vkGetSwapchainImagesKHR(deviceHandle,swapchainHandle,&cnt,nullptr) != VK_SUCCESS)
		throw std::runtime_error("failed to get swapchain images count.");
	
	swapchainImages.resize(cnt);
	if(vkGetSwapchainImagesKHR(deviceHandle,swapchainHandle,&cnt,swapchainImages.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to get swapchain images.");
	
	swapchainImageViews.resize(cnt);
	for(size_t i=0; i<cnt; ++i)
		swapchainImageViews[i] = rendererUtility::createImageView(*this,swapchainImages[i],surfaceFormatHandle.format,VK_IMAGE_ASPECT_COLOR_BIT,1);
	currentImagesInFlight.clear();
	currentImagesInFlight.resize(swapchainImages.size(),VK_NULL_HANDLE);
}
void renderer::initializeRenderPass(){
}
void renderer::initializeFramebuffers(){
}
void renderer::allocateCommandBuffers(){
}
