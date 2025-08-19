#include "vk_shaders_manager.h"

#include <filesystem>
#include "drivers/vulkan/tools/vk_read_shaders.h"


namespace Driver::Vulkan::Shader {

void VulkanShaderManager::Release(const VkDevice& device) {
	for (auto module : shadermodules_) {
		vkDestroyShaderModule(device, module.second, nullptr);
	}
  
}

// VkShaderModule VulkanShaderManager::createShaderModule(const std::vector<char>& code){
// 	VkShaderModuleCreateInfo createInfo{};
// 	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
// 	createInfo.codeSize = code.size();
// 	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

// 	VkShaderModule shaderModule;
// 	if (vkCreateShaderModule(GetDevice(device_), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
// 		LogErrorDetaill("[Vulkan][Pipeline] Fail To Create ShaderModule");
// 	}
// 	return shaderModule;
// }



// VkShaderModule VulkanShaderManager::LoadShader(std::string_view shader) {
// 	namespace fs = std::filesystem;
//     fs::path fullPath = fs::path(shaderpath_) / fs::path(shader);
// 	auto future = Tools::ReadShaderFileAsync(fullPath.string());

//     std::vector<char> code = future.get();
    

// }

} //namespace Driver::Vulkan::Shader