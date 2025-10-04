#ifndef SG_VULKAN_SHADER_MANAGER_H
#define SG_VULKAN_SHADER_MANAGER_H
#include <volk.h>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "core/async/coroutine/task/file_read.h"
#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan::Shader {

using TaskFile = Core::Async::CoroutineGenerator<std::vector<char>, Core::Async::CoroutinePolicy::Await>;
using TaskModule = Core::Async::CoroutineGenerator<VkShaderModule, Core::Async::CoroutinePolicy::Await>;
using TaskTwoModule = Core::Async::CoroutineGenerator<std::pair<VkShaderModule,VkShaderModule>, Core::Async::CoroutinePolicy::Await>;

class VulkanShaderManager {

	using s_callback = std::function<void(VkShaderModule)>;
	using t_callback = std::function<void(VkShaderModule,VkShaderModule)>;
public:
	VulkanShaderManager(std::string shaderPath = "Assets/Shaders/") :
			shaderpath_(std::move(shaderPath)) {}
	~VulkanShaderManager() = default;
public:
	void Release(const VkDevice&);
	VkShaderModule LoadShader(const VkDevice&, std::string_view);

	//Async(Coroutine)
	TaskModule LoadShaderModuleAsync(const VkDevice&, std::string_view,s_callback = nullptr);
	TaskTwoModule LoadShaderModuleAsync(const VkDevice&, std::string_view,std::string_view,t_callback = nullptr);
private:
	VkShaderModule createShaderModule(const VkDevice&, const std::vector<char>&);
private:
	std::string shaderpath_;
	std::unordered_map<std::string, VkShaderModule> shadermodules_;
};

} //namespace Driver::Vulkan::Shader

#endif