#include "vk_instance.h"

#define VK_NO_PROTOTYPES
#define VOLK_IMPLEMENTATION
#include "volk.h"

#include "core/io/log/log.h"
#include "extensions/vk_check.h"
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
			VK_LOG_LAYER_INFO("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
			return false;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			VK_LOG_LAYER_INFO("[Vulkan][{}]: {}", type_name, callback_data->pMessage);
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

VkInstance GetInstance(const VulkanInitializer& init) {
	return init.GetInstance();
}

VkPhysicalDevice GetPhysicalDevice(const VulkanInitializer& init) {
	return init.GetPhysicalDevice();
}

VulkanInitializer::VulkanInitializer() {
	Init();
}

VulkanInitializer::~VulkanInitializer() noexcept {
	vkDestroyInstance(instance_, nullptr);
	LogInfo("[Vulkan][Destory] Destory Instance");
}

void VulkanInitializer::Init() {
	if (volkInitialize() != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Instance] Failed to initialize Volk");
	}

	InitVulkanInstance();
	LogInfo("[Vulkan][Init] Instance Init Success");
	PickPhysicalDevice();
	LogInfo("[Vulkan][Init] Physical Device Init Success");
}

void VulkanInitializer::InitVulkanInstance() {
	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = "Sago";
	app_info.pEngineName = "Sago";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_4;

	VkInstanceCreateInfo create_Info{};
	create_Info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_Info.pApplicationInfo = &app_info;

	uint32_t extension_count{};
	auto sdl_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);

	if (sdl_instance_extensions == nullptr) {
		LogErrorDetail("[Vulkan][Instance] Failed to Get SDL Vulkan Extensions");
	}

	std::vector<const char*> required_extension;
	for (int i = 0; i < extension_count; ++i) {
		required_extension.emplace_back(sdl_instance_extensions[i]);
	}

	//BudGet
	required_extension.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#if defined(__APPLE__)
	required_extension.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	create_Info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

#if defined(NDEBUG)
	const bool enableValidationLayers = false;
#else

	LogInfo("[Vulkan][Layer] Enable Validation Layer");

	const bool enableValidationLayers = true;
	auto check = CheckVulkanSupport<CheckType::kValidationLayer>(validationLayers);
	if (enableValidationLayers && !check) {
		LogErrorDetail("[Vulkan][Instance] ValidationLayer not supported: ");
	}
	required_extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	//required_extension.push_back(VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME);

	if (!CheckVulkanSupport<CheckType::kInstanceRequireExtensions>(required_extension)) {
		LogErrorDetail("[Vulkan][Instance] InstanceRequireExtensions not supported: ");
	}

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
	create_Info.pNext = &debug_ci;
#endif

	create_Info.enabledExtensionCount = required_extension.size();
	create_Info.ppEnabledExtensionNames = required_extension.data();

	if (enableValidationLayers) {
		create_Info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		create_Info.ppEnabledLayerNames = validationLayers.data();
	} else {
		create_Info.enabledLayerCount = 0;
	}

	if (auto result = vkCreateInstance(&create_Info, nullptr, &instance_); result != VK_SUCCESS) {
		VK_LOG_ERROR("Failed to create instance: ", result);
	}

	volkLoadInstance(instance_);
}

void VulkanInitializer::PickPhysicalDevice() {
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
	if (device_count == 0) {
		LogErrorDetail("[Vulkan][Instance] Failed to find GPUs");
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

	for (const auto& device : devices) {
		if (CheckVulkanSupport<CheckType::kPhysicalDeviceSuitable>(device)) {
			physical_device_ = device;
			break;
		}
	}
	
}

} //namespace Driver::Vulkan
