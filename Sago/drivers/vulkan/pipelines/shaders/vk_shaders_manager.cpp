#include "vk_shaders_manager.h"

#include <filesystem>

#include "core/io/log/log.h"
#include "drivers/vulkan/tools/vk_read_shaders.h"

namespace Driver::Vulkan::Shader {

void VulkanShaderManager::Release(const VkDevice& device) {
	for (auto module : shadermodules_) {
		vkDestroyShaderModule(device, module.second, nullptr);
	}
}

VkShaderModule VulkanShaderManager::createShaderModule(const VkDevice& device, const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][Shader]: Fail To Create ShaderModule: {}", shaderpath_);
	}
	return shaderModule;
}

VkShaderModule VulkanShaderManager::LoadShader(const VkDevice& device, std::string_view shader) {
	namespace fs = std::filesystem;
	fs::path fullPath = fs::path(shaderpath_) / fs::path(shader);
	auto code = Tools::ReadShaderFile(fullPath.string());
	auto mode = createShaderModule(device, code);
	shadermodules_.emplace(shader, mode);
	return mode;
}

//Corutine Async
TaskModule VulkanShaderManager::LoadShaderModuleAsync(const VkDevice& device, std::string_view shaderName) {
	using namespace Core::Async;

	namespace fs = std::filesystem;
	fs::path fullPath = fs::path(shaderpath_) / fs::path(shaderName);
	FileReader reader(fullPath.string());
	std::vector<char> code = co_await reader;

	if (code.empty()) {
		LogErrorDetaill("Failed to load shader: {}", shaderName);
		co_return VK_NULL_HANDLE;
	}

	VkShaderModule module = createShaderModule(device, code);
	shadermodules_.emplace(std::string(shaderName), module);

	LogInfo("[Vulkan][Shader]: loaded successfully: {}", shaderName);
	std::cout << "loaded successfully \n";
	co_return module;
}

} //namespace Driver::Vulkan::Shader