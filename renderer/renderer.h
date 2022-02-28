#include "window/window.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <array>

class renderer{
public:
	struct queueFamilyIndices{
		std::optional<uint32_t> graphicsFamilyIndex;
		std::optional<uint32_t> presentationFamilyIndex;
	};
private:
	struct frameSynchronizationPrimitives{
		VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
		VkFence inFlightFence = VK_NULL_HANDLE;
	};

	VkInstance instanceHandle = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDeviceHandle = VK_NULL_HANDLE;
	VkDevice deviceHandle = VK_NULL_HANDLE;
	VkCommandPool commandPoolHandle = VK_NULL_HANDLE;
	VkQueue graphicsQueueHandle = VK_NULL_HANDLE;
	VkQueue presentationQueueHandle = VK_NULL_HANDLE;
	static constexpr std::array<const char*,1> deviceExtensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkPipelineCache pipelineCacheHandle = VK_NULL_HANDLE;
	
	std::vector<frameSynchronizationPrimitives> perFrameSynchronization;
	
public:
	renderer(GLFWwindow* windowHandle);
	~renderer();
	void initializeInstance();
	void initializeSurface(GLFWwindow* windowHandle);
	void initializeDevice();
	void initializeCommandPool();
	void initializePipelineCache();	
	void initializeSynchronizationPrimitives();
	void initializePresentationObjects();

	//utils
	queueFamilyIndices indices{};
	std::vector<const char*> getInstanceExtensions() const;
	void pickPhysicalDevice();
	void fillInQueueFamilyIndices();

	//presentation Objects
	void initializeSwapchain();
	void initializeRenderPass();
	void initializeFramebuffers();
	void allocateCommandBuffers();

};
