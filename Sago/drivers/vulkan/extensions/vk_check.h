#ifndef SG_VULKAN_EXTENSION_CHECK_H
#define SG_VULKAN_EXTENSION_CHECK_H
#include <volk.h>
#include <vector>

namespace Driver::Vulkan {

enum class CheckType {
	kInstanceRequireExtensions,
	kValidationLayer,
	kPhysicalDeviceSuitable,
	kDeviceExtensions,
	kSurfaceSupport,
};


bool CheckInstanceRequireExtensionsSupport(const std::vector<const char*>&);
bool CheckValidationLayerSupport(const std::vector<const char*>&);
bool CheckPhysicalDeviceSuitable(const VkPhysicalDevice&);
bool CheckDeviceExtensionsSupport(const std::vector<const char*>&, const VkPhysicalDevice&);
bool CheckSurfaceSupport(const VkPhysicalDevice&, const VkSurfaceKHR&);

template <CheckType type, typename... Args>
bool CheckVulkanSupport(Args&&... args) = delete;

template <CheckType type, typename Arg1, typename Arg2>
bool CheckVulkanSupport(Arg1&& arg1, Arg2&& arg2) = delete;

template <>
inline bool CheckVulkanSupport<CheckType::kInstanceRequireExtensions>(std::vector<const char*>& require) {
	return CheckInstanceRequireExtensionsSupport(require);
}

template <>
inline bool CheckVulkanSupport<CheckType::kValidationLayer>(const std::vector<const char*>& layers) {
	return CheckValidationLayerSupport(layers);
}

template <>
inline bool CheckVulkanSupport<CheckType::kPhysicalDeviceSuitable>(const VkPhysicalDevice& pdevice) {
	return CheckPhysicalDeviceSuitable(pdevice);
}

template <>
inline bool CheckVulkanSupport<CheckType::kDeviceExtensions>(const std::vector<const char*>& extens, const VkPhysicalDevice& pdevice) {
	return CheckDeviceExtensionsSupport(extens, pdevice);
}

template <>
inline bool CheckVulkanSupport<CheckType::kSurfaceSupport>(const VkPhysicalDevice& pdevice,const VkSurfaceKHR& surface) {
	return CheckSurfaceSupport(pdevice, surface);
}




} //namespace Driver::Vulkan

#endif