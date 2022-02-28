#include "window/window.h"
#include <vulkan/vulkan.h>
#include <vector>

class renderer{
private:
	VkInstance instanceHandle = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDeviceHandle = VK_NULL_HANDLE;
	VkDevice deviceHandle = VK_NULL_HANDLE;
	VkCommandPool commandPoolHandle = VK_NULL_HANDLE;
	VkQueue graphicsQueueHandle = VK_NULL_HANDLE;
	VkQueue presentationQueueHandle = VK_NULL_HANDLE;
	
public:
	renderer(GLFWwindow* windowHandle);
	~renderer();
	void initializeInstance();
	void initializeSurface(GLFWwindow* windowHandle);
	void initializeDevice();
	void initializeCommandPool();

	std::vector<const char*> getInstanceExtensions() const;
};
