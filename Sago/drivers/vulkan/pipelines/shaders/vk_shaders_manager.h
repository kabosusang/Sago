#ifndef SG_VULKAN_SHADER_MANAGER_H
#define SG_VULKAN_SHADER_MANAGER_H
#include <volk.h>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Driver::Vulkan::Shader {


class VulkanShaderManager {
public:
	VkShaderModule LoadShader(std::string_view);
	

	void Release(const VkDevice&);
	VulkanShaderManager(std::string shaderPath = "Assets/Shaders/") 
        : shaderpath_(std::move(shaderPath)) {}

	~VulkanShaderManager();
private:
	VkShaderModule createShaderModule(const std::vector<char>&);

private:
	std::string shaderpath_;
	std::unordered_map<std::string, VkShaderModule> shadermodules_;
};

} //namespace Driver::Vulkan::Shader

#endif