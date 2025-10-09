#ifndef SG_VK_DESCRITPOR_H
#define SG_VK_DESCRITPOR_H
#include <volk.h>

#include "core/io/log/log.h"
#include <cstdint>
#include <vector>

namespace Driver::Vulkan {

class VulkanDescriptor {
public:
	VulkanDescriptor(VkDevice device, std::uint32_t bind = 0, uint32_t bindcount = 1, VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
		VkDescriptorSetLayoutBinding layout_binding{};
		layout_binding.binding = bind;
		layout_binding.descriptorCount = bindcount;
		layout_binding.descriptorType = type;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindcount;
		layoutInfo.pBindings = &layout_binding;

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout_) != VK_SUCCESS) {
			LogError("[Vulkan][Descriptor]: Failed to create descriptor set layout!");
		}
		/*
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		*/
	}

	void Clean(VkDevice device) const noexcept{
		vkDestroyDescriptorSetLayout(device, layout_, nullptr);
	}

private:
	VkDescriptorSetLayout layout_;
};


class VulkanDescriptorPool{
public:
    VulkanDescriptorPool(VkDevice device,uint32_t frameflights = 2,VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) :device_(device){
        VkDescriptorPoolSize pool_size{};
        pool_size.type = type;
        pool_size.descriptorCount = frameflights;

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = 1;
        pool_info.pPoolSizes = &pool_size;
        pool_info.maxSets = static_cast<uint32_t>(frameflights);

        if (vkCreateDescriptorPool(device, &pool_info, nullptr, &pool_) != VK_SUCCESS) {
            LogError("[Vulkan][Descriptor]: Failed to create descriptor pool!");
        }
    }

   ~VulkanDescriptorPool(){
        vkDestroyDescriptorPool(device_, pool_, nullptr);
   }

public:
   
private:
   VkDevice device_;
   VkDescriptorPool pool_;
   std::vector<VkDescriptorSet> sets_;
};



























} //namespace Driver::Vulkan

#endif