#include "vk_queue_faimly.h"

#include <vector>

namespace Driver::Vulkan {

QueueFamilyIndices FindQueueFamilise(const VkPhysicalDevice& device) {
	QueueFamilyIndices indices;

	uint32_t queue_family_Count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_Count, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_Count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_Count, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicis_family_ = i;
		}

		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;
}

} //namespace Driver::Vulkan
