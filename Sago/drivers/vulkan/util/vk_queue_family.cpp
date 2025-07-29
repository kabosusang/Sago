#include "vk_queue_faimly.h"

#include <variant>
#include <vector>

namespace Driver::Vulkan {

auto Graphy::operator()(const VkPhysicalDevice& device) const {
	QueueFamilyIndice indices;

	uint32_t queue_family_Count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_Count, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_Count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_Count, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.family_ = i;
		}
		if (indices.isComplete()) {
			break;
		}
		i++;
	}

	return indices;
}

auto Presente::operator()(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const {
	QueueFamilyIndice indices;
	uint32_t queue_family_Count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_Count, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queue_family_Count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_Count, queueFamilies.data());

	int i = 0;

	for (const auto& queueFamily : queueFamilies) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport) {
			indices.family_ = i;
		}
		if (indices.isComplete()) {
			break;
		}
		i++;
	}
	return indices;
}

QueueFamilyIndice FindIndice(const QueueFamilyStrategy& strategy,
		const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
	return std::visit(
			[&](auto&& s) -> QueueFamilyIndice {
				if constexpr (std::is_invocable_v<decltype(s), const VkPhysicalDevice&>) {
					return s(device);
				} else if constexpr (std::is_invocable_v<decltype(s), const VkPhysicalDevice&, const VkSurfaceKHR&>) {
					return s(device, surface);
				}
			},
			strategy);
}

} //namespace Driver::Vulkan
