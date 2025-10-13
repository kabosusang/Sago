#ifndef SG_VK_DESCRITPOR_H
#define SG_VK_DESCRITPOR_H
#include <volk.h>

#include "core/io/log/log.h"
#include <cstddef>
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

	operator VkDescriptorSetLayout() const noexcept { return layout_; }

	void Clean(VkDevice device) const noexcept {
		vkDestroyDescriptorSetLayout(device, layout_, nullptr);
	}

private:
	VkDescriptorSetLayout layout_;
};

class VulkanDescriptorPool {
public:
	VulkanDescriptorPool(VkDevice device, const VulkanDescriptor& layout, uint32_t frameflights = 2, VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) :
			device_(device), layout_(layout) {
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

		std::vector<VkDescriptorSetLayout> layouts(frameflights, layout_);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool_;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(frameflights);
		allocInfo.pSetLayouts = layouts.data();

		//set
		sets_.resize(frameflights);
		if (vkAllocateDescriptorSets(device, &allocInfo, sets_.data()) != VK_SUCCESS) {
			LogError("[Vulkan][Descriptor]: Failed to allocate descriptor sets!");
		}
	}

	~VulkanDescriptorPool() {
		vkDestroyDescriptorPool(device_, pool_, nullptr);
		layout_.Clean(device_);
	}

	template <class Object>
	void CreateDescriptorSets(std::vector<VkBuffer>& uniformBuffers) {
		for (size_t i = 0; i < sets_.size(); i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(Object);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = sets_[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(device_, 1, &descriptorWrite, 0, nullptr);
		}
	}

public:
private:
	const VulkanDescriptor& layout_;

private:
	VkDevice device_;
	VkDescriptorPool pool_;
	std::vector<VkDescriptorSet> sets_;
};

#define ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR))))

template <size_t maxSet, VkDescriptorPoolCreateFlags flag = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT>
class VulkanDescriptorPoolNoSet {
public:
	VulkanDescriptorPoolNoSet(VkDevice device, VkDescriptorType type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) :
			device_(device) {
		VkDescriptorPoolSize pool_size[] = {
			{ type, maxSet }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = (uint32_t)ARRAYSIZE(pool_size);
		pool_info.pPoolSizes = pool_size;
		pool_info.maxSets = maxSet;
		pool_info.flags = flag;

		if (vkCreateDescriptorPool(device, &pool_info, nullptr, &pool_) != VK_SUCCESS) {
			LogError("[Vulkan][Descriptor]: Failed to create descriptor pool!");
		}
	}

	~VulkanDescriptorPoolNoSet(){
		vkDestroyDescriptorPool(device_, pool_, nullptr);
	}

	operator VkDescriptorPool() const { return pool_; }
	VkDescriptorPool GetPool() const { return pool_; }

private:
	VkDevice device_;
	VkDescriptorPool pool_;
};

} //namespace Driver::Vulkan

#endif