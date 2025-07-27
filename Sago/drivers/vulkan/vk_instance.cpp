#include "vk_instance.h"

#define VOLK_IMPLEMENTATION
#include "volk.h"

#include "core/io/log/log.h"
#include "vk_log.h"

namespace Driver::Vulkan {

static VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT level,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data) {
	std::string type_name;

	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
		type_name += "General";
	}
	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
		type_name += type_name.empty() ? "Validation" : "|Validation";
	}
	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
		type_name += type_name.empty() ? "Performance" : "|Performance";
	}
	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
		type_name += type_name.empty() ? "DeviceAddressBinding"
									   : "|DeviceAddressBinding";
	}

	switch (level) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LogInfo("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
			return false;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LogInfo("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
			return false;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LogWarring("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
			return false;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LogError("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
			return true;
		default:
			break;
	}

	return false;
}

VulkanInitializer::VulkanInitializer() {
}

VulkanInitializer::~VulkanInitializer() noexcept {
	vkDestroyInstance(instance_, nullptr);
	LogInfo("Vulkan Destory Instance");
}

void VulkanInitializer::Init() {
	if (volkInitialize() != VK_SUCCESS) {
		LogErrorDetaill("Failed to initialize Volk");
	}

	InitVulkanInstance();
	LogInfo("Vulkan Instance Init");
	PickPhysicalDevice();
	LogInfo("Vulkan Physical Device Init");
}

void VulkanInitializer::InitVulkanInstance() {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "Sago";
	appInfo.pEngineName = "Sago";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t extension_count{};
	auto sdl_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);

	if (sdl_instance_extensions == nullptr) {
		LogErrorDetaill("Failed to Get SDL Vulkan Extensions");
	}

	std::vector<const char*> requiredextension;
	for (int i = 0; i < extension_count; ++i) {
		requiredextension.emplace_back(sdl_instance_extensions[i]);
	}

#if defined(__APPLE__)
	requiredextension.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

#if defined(NDEBUG)
	const bool enableValidationLayers = false;
#else
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	LogInfo("Vulkan Enable Validation Layer");

	const bool enableValidationLayers = true;
	CheckRequireDextensionSupport(requiredextension);
	if (enableValidationLayers && !CheckValidationLayerSupport(validationLayers)) {
		LogErrorDetaill("ValidationLayer not supported: ");
	}
	requiredextension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	//requiredextension.push_back(VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME);
	
	VkDebugUtilsMessengerCreateInfoEXT debug_ci{};
	debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debug_ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debug_ci.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			//VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debug_ci.pfnUserCallback = VulkanDebugCallback;
	debug_ci.pUserData = nullptr;
	createInfo.pNext = &debug_ci;
#endif

	createInfo.enabledExtensionCount = requiredextension.size();
	createInfo.ppEnabledExtensionNames = requiredextension.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	auto result = vkCreateInstance(&createInfo, nullptr, &instance_);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("Failed to create instance", result);
	}

	volkLoadInstance(instance_);
}

void VulkanInitializer::PickPhysicalDevice() {
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
	if (device_count == 0) {
		LogErrorDetaill("Failed to find GPUs");
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

	for (const auto& device : devices) {
		if (CheckIsDeviceSuitable(device)) {
			physical_device_ = device;
			break;
		}
	}
}

void VulkanInitializer::CheckRequireDextensionSupport(std::vector<const char*>& required) const {
	uint32_t available_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &available_count, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(available_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &available_count, availableExtensions.data());

	for (const auto& reqExt : availableExtensions) {
		bool found = false;
		for (const auto& availExt : availableExtensions) {
			if (strcmp(reqExt.extensionName, availExt.extensionName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			LogErrorDetaill("Extension not supported: {}", reqExt.extensionName);
		}
	}
}

bool VulkanInitializer::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers) const {
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> availableLayers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, availableLayers.data());

	for (const char* layerName : validationLayers) {
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

bool VulkanInitializer::CheckIsDeviceSuitable(const VkPhysicalDevice& device) const {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	LogInfo("Vulkan Select GPU: {}", deviceProperties.deviceName);

	QueueFamilyIndices indices = FindQueueFamilise(device);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader &&
			indices.isComplete();
}

QueueFamilyIndices FindQueueFamilise(const VkPhysicalDevice& device){
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
