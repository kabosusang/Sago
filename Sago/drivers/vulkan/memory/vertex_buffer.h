#ifndef SG_VULKAN_MEMORY_VERTEXBUFFER_H
#define SG_VULKAN_MEMORY_VERTEXBUFFER_H

#include "vertex.h"
#include "vk_memory_requirement.h"

#include "core/io/log/log.h"

namespace Driver::Vulkan::Memory {

template <typename vertex, size_t N>
class VulkanVertexBuffer {
public:
	VulkanVertexBuffer(const VkPhysicalDevice& phy, const VkDevice& device) :
			phydevice_(phy),
			device_(device) {
		CreateVertexBuffer();
	}
	~VulkanVertexBuffer() {
		vkDestroyBuffer(device_, vertex_buffer_, nullptr);
		vkFreeMemory(device_, vertex_buffer_memory_, nullptr);
	}

private:
	void CreateVertexBuffer();

private:
	const VkPhysicalDevice& phydevice_;
	const VkDevice& device_;
	VkBuffer vertex_buffer_;
	VkDeviceMemory vertex_buffer_memory_;
};

template <typename vertex, size_t N>
void VulkanVertexBuffer<vertex, N>::CreateVertexBuffer() {
	VkBufferCreateInfo buff_info{};
	buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buff_info.size = sizeof(vertex) * N;
	buff_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	buff_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device_, &buff_info, nullptr, &vertex_buffer_) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][VertexBuffer]: Failed to to create vertex buffer!");
	}

	auto requirement = Requirement::MemoryRequirements(device_, vertex_buffer_);
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = requirement.size;
	allocInfo.memoryTypeIndex = Requirement::FindMemoryType(phydevice_, requirement.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device_, &allocInfo, nullptr, &vertex_buffer_memory_) != VK_SUCCESS) {
		//throw std::runtime_error("failed to allocate vertex buffer memory!");
		LogErrorDetail("[Vulkan][VertexBuffer]: failed to allocate vertex buffer memory!");
	}

	const std::vector<Vertex> vertices = {
		{ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
	};

	vkBindBufferMemory(device_, vertex_buffer_, vertex_buffer_memory_, 0);
	void* data;
	vkMapMemory(device_, vertex_buffer_memory_, 0, buff_info.size, 0, &data);
	memcpy(data, vertices.data(), (size_t)buff_info.size);
	vkUnmapMemory(device_, vertex_buffer_memory_);
}

} //namespace Driver::Vulkan::Memory

#endif