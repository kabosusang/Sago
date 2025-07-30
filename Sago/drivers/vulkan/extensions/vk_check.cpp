#include "vk_check.h"


#include <set>
#include <string>

#include "core/io/log/log.h"
#include "drivers/vulkan/util/vk_queue_faimly.h"

namespace Driver::Vulkan {

bool CheckInstanceRequireExtensionsSupport(const std::vector<const char*>& required) {
	uint32_t available_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &available_count, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(available_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &available_count, availableExtensions.data());

	for (const auto& reqExt : required) {
		bool found = false;
		for (const auto& availExt : availableExtensions) {
			if (strcmp(reqExt, availExt.extensionName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

bool CheckValidationLayerSupport(const std::vector<const char*>& layers) {
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> availableLayers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());

	for (const char* layerName : layers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}
	return true;
}

bool CheckPhysicalDeviceSuitable(const VkPhysicalDevice& pdevice) {
	VkPhysicalDeviceProperties device_properties{};
	VkPhysicalDeviceFeatures device_features{};
	vkGetPhysicalDeviceProperties(pdevice, &device_properties);
	vkGetPhysicalDeviceFeatures(pdevice, &device_features);

	LogInfo("[Vulkan][GPU] Select GPU: {}", device_properties.deviceName);

	auto indices = FindIndice(Graphy{}, pdevice);
	return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			device_features.geometryShader &&
			indices.isComplete();
}

bool CheckDeviceExtensionsSupport(const std::vector<const char*>& deviceExtensions,const VkPhysicalDevice& device) {
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : available_extensions) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}

bool CheckSurfaceSupport(const VkPhysicalDevice& pdevice, const VkSurfaceKHR& surface) {
	auto indice_present = FindIndice(Presente{}, pdevice, surface);
	return indice_present.isComplete();
}

} //namespace Driver::Vulkan