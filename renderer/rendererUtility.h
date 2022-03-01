#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "renderer/renderer.h"

namespace vulkanUtility{
	[[nodiscard]] VkImageView createImageView(const renderer& r,const VkImage image,const VkFormat format,
						const VkImageAspectFlags imageAspect,const std::uint32_t mipLevels);
}
