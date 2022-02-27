#include "window/window.h"
#include <vulkan/vulkan.h>


class renderer{
private:
	VkInstance instanceHandle = VK_NULL_HANDLE;
	VkSurfaceKHR SurfaceHandle = VK_NULL_HANDLE;
	VkPhysicalDevice PhysicalDeviceHandle = VK_NULL_HANDLE;
	VkDevice deviceHandle = VK_NULL_HANDLE;
	VkCommandPool commandPoolHandle = VK_NULL_HANDLE;
	VkQueue graphicsQueueHandle = VK_NULL_HANDLE;
	VkQueue presentationQueueHandle = VK_NULL_HANDLE;
	
public:
	renderer(GLFWwindow* windowHandle);
	~renderer();
	void initializeInstance();
	void initializeSurface();
	void initializeDevice();
	void initializeCommandPool();
};
