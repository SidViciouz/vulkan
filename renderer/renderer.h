#pragma once
#include "window/window.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <array>

class renderer{
public:
	struct config{
		bool enableVsync = true;
		bool enableMultisample = true;
	};
	struct queueFamilyIndices{
		std::optional<uint32_t> graphicsFamilyIndex;
		std::optional<uint32_t> presentationFamilyIndex;
	};
	struct surfaceProperties{
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentationModes;
	};
private:
	struct frameSynchronizationPrimitives{
		VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
		VkFence inFlightFence = VK_NULL_HANDLE;
	};
	
	GLFWwindow* wHandle;
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

	bool enableVsyncHandle = true;
	bool enableMultisampleHandle = true;
	VkPipelineCache pipelineCacheHandle = VK_NULL_HANDLE;
	std::vector<frameSynchronizationPrimitives> perFrameSynchronization;
	std::vector<VkFence> currentImagesInFlight;
	size_t currentFrameInFlight = 0;

	VkSurfaceFormatKHR surfaceFormatHandle{};
	VkPresentModeKHR presentationModeHandle = VK_PRESENT_MODE_FIFO_KHR;
	VkExtent2D swapchainExtentHandle{};
	VkSwapchainKHR swapchainHandle = VK_NULL_HANDLE;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;	
	
public:
	renderer(GLFWwindow* windowHandle,const renderer::config& configHandle);
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
	[[nodiscard]] surfaceProperties getSurfaceProperties() const;
	[[nodiscard]] VkSurfaceFormatKHR getBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats) const;
	[[nodiscard]] VkPresentModeKHR getBestPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes) const;
	[[nodiscard]] VkExtent2D getDrawableSurfaceExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const; 
	inline VkDevice getDevice() const noexcept {return deviceHandle;}
	
	//presentation Objects
	void initializeSwapchain();
	void initializeSwapchainImages();
	void initializeRenderPass();
	void initializeFramebuffers();
	void allocateCommandBuffers();

};
