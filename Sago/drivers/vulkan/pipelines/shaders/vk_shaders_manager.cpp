#include "vk_shaders_manager.h"

#include <filesystem>
#include <future>

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
		LogErrorDetail("[Vulkan][Shader]: Fail To Create ShaderModule: {}", shaderpath_);
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
TaskModule VulkanShaderManager::LoadShaderModuleAsync(const VkDevice& device, std::string_view shaderName, s_callback callback) {
	using namespace Core::Async;
	namespace fs = std::filesystem;

	fs::path fullPath = fs::path(shaderpath_) / fs::path(shaderName);
	FileReader reader(fullPath.string());
	std::vector<char> code = co_await reader;

	if (code.empty()) {
		LogErrorDetail("Failed to load shader: {}", shaderName);
		co_return VK_NULL_HANDLE;
	}

	VkShaderModule module = createShaderModule(device, code);
	shadermodules_.emplace(std::string(shaderName), module);
	LogInfo("[Vulkan][Shader]: loaded successfully: {}", shaderName);
	callback(module);

	co_return module;
}

TaskTwoModule VulkanShaderManager::LoadShaderModuleAsync(const VkDevice& device, std::string_view vert, std::string_view frag, t_callback callback) {
	using namespace Core::Async;
	namespace fs = std::filesystem;

	fs::path vertPath = fs::path(shaderpath_) / fs::path(vert);
	fs::path fragPath = fs::path(shaderpath_) / fs::path(frag);

	//vert
	FileReader vert_reader(vertPath.string());
	std::vector<char> code = co_await vert_reader;

	VkShaderModule module_vert = createShaderModule(device, code);
	shadermodules_.emplace(std::string(vert), module_vert);
	LogInfo("[Vulkan][Shader]: loaded successfully: {}", vert);

	//frag
	FileReader frag_reader(fragPath.string());
	code.clear();
	code = co_await frag_reader;

	auto module_frag = createShaderModule(device, code);
	shadermodules_.emplace(std::string(frag), module_frag);
	LogInfo("[Vulkan][Shader]: loaded successfully: {}", frag);
	callback(module_vert, module_frag);

	co_return { module_vert, module_frag };
}

} //namespace Driver::Vulkan::Shader