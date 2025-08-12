#include "vk_shaders_manager.h"

namespace Driver::Vulkan::Shader {

VkShaderModule VulkanShaderManager::LoadShader(std::string_view filename){



    
}

void VulkanShaderManager::Release(const VkDevice& device){
    for (auto module : shadermodules_){
        vkDestroyShaderModule(device,module.second,nullptr);
    }
}





} //namespace Driver::Vulkan::Shader