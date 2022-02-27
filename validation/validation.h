#pragma once

#include <vulkan/vulkan.h>
#include <array>

class validation{

private:
#if defined(NDEBUG)
	static constexpr bool enableValidationLayer = false;
#else
	static constexpr bool enableValidationLayer = true;
#endif

	static constexpr std::array<const char*,1u> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
	inline static VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;
public:
	validation() = delete;
	~validation() noexcept = delete;
	
	static constexpr bool areEnabled() noexcept { return enableValidationLayer;}
	static decltype(validationLayers)& getValidationLayers() noexcept {return validationLayers;}
	static bool areSupported();
	
	static VkResult createDebugUtilsMessenger(const VkInstance instance,const VkDebugUtilsMessengerCreateInfoEXT& Info);
	static void fillInCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& Info);
	static void setUpDebugUtilsMessenger(const VkInstance instance);
	static void destroyDebugUtilsMessenger(const VkInstance instance) noexcept;
private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,const VkDebugUtilsMessageTypeFlagsEXT messageType,const VkDebugUtilsMessengerCallbackDataEXT* const callbackData,void* const userData) noexcept;
};
