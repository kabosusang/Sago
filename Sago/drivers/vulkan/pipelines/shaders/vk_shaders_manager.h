#ifndef SG_VULKAN_SHADER_MANAGER_H
#define SG_VULKAN_SHADER_MANAGER_H
#include <volk.h>
#include <string>
#include <string_view>
#include <unordered_map>

#include "core/async/coroutine/task/file_read.h"
#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan::Shader {

using TaskFile = Core::Async::CoroutineGenerator<std::vector<char>, Core::Async::CoroutinePolicy::Await>;
using TaskModule = Core::Async::CoroutineGenerator<VkShaderModule, Core::Async::CoroutinePolicy::Await>;

class VulkanShaderManager {
public:
	VulkanShaderManager(std::string shaderPath = "Assets/Shaders/") :
			shaderpath_(std::move(shaderPath)) {}
	~VulkanShaderManager() = default;
public:
	void Release(const VkDevice&);
	VkShaderModule LoadShader(const VkDevice&, std::string_view);
	//Async
	TaskModule LoadShaderModuleAsync(const VkDevice&, std::string_view);
private:
	VkShaderModule createShaderModule(const VkDevice&, const std::vector<char>&);
	//Async
	TaskFile LoadShaderFileAsync(std::string_view);
private:
	std::string shaderpath_;
	std::unordered_map<std::string, VkShaderModule> shadermodules_;
};

} //namespace Driver::Vulkan::Shader

#endif