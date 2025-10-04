#ifndef SG_VULKAN_MEMORY_VMAALLOCATOR_H
#define SG_VULKAN_MEMORY_VMAALLOCATOR_H
#include <cstdint>
#include <string>
#include <vector>

#include <volk.h>
#include <vk_mem_alloc.h>


namespace Driver::Vulkan::Memory {
struct AllocatorConfig {
	uint32_t vulkan_apiVersion = VK_API_VERSION_1_4;
	VmaAllocatorCreateFlags flags = 0;
	bool enable_memorybudget = true;
	bool enable_bufferdevice_address = true;
	bool enable_debugutils = false;
};

class VulkanAllocator {
public:
	enum class BufferPreset {
		Default,
		Vertex,
		Index,
		Uniform,
		Storage,
		Staging
	};

	struct Buffer {
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VmaAllocationInfo allocation_info = {};
		std::string debug_name;

		bool IsValid() const noexcept {
			return buffer != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE;
		}

		void* GerMappedData() const noexcept {
			return allocation_info.pMappedData;
		}

		bool IsMapped() const noexcept { return allocation_info.pMappedData != nullptr; }
		VkDeviceSize GetSize() const noexcept { return allocation_info.size; }

		void Reset() noexcept {
			buffer = VK_NULL_HANDLE;
			allocation = VK_NULL_HANDLE;
			allocation_info = {};
			debug_name.clear();
		}
	};

	struct Image {
		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VmaAllocationInfo allocation_info = {};
		std::string debug_name;

		bool IsValid() const noexcept { return image != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE; }
		VkDeviceSize GetSize() const noexcept { return allocation_info.size; }

		void Reset() noexcept {
			image = VK_NULL_HANDLE;
			allocation = VK_NULL_HANDLE;
			allocation_info = {};
			debug_name.clear();
		}
	};

public:
	Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
			VmaMemoryUsage memoryUsage,
			VmaAllocationCreateFlags allocationFlags = 0,
			const std::string& debugName = "");

	template <typename VertexType>
	Buffer CreateVertexBuffer(size_t vertexCount, const std::string& debugName = "") {
		return CreateBuffer(
				sizeof(VertexType) * vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VMA_MEMORY_USAGE_GPU_ONLY,
				0,
				debugName.empty() ? "VertexBuffer" : debugName);
	}

	template <typename IndexType>
	Buffer CreateIndexBuffer(size_t indexCount, const std::string& debugName = "") {
		return CreateBuffer(
				sizeof(IndexType) * indexCount,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VMA_MEMORY_USAGE_GPU_ONLY,
				0,
				debugName.empty() ? "IndexBuffer" : debugName);
	}

	template <typename T>
	Buffer CreateUniformBuffer(bool hostVisible = true, const std::string& debugName = "") {
		VmaMemoryUsage memoryUsage = hostVisible ? VMA_MEMORY_USAGE_CPU_TO_GPU : VMA_MEMORY_USAGE_GPU_ONLY;
		VmaAllocationCreateFlags flags = hostVisible ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;

		return CreateBuffer(
				sizeof(T),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				memoryUsage,
				flags,
				debugName.empty() ? "UniformBuffer" : debugName);
	}

	template <typename T>
	Buffer CreateStorageBuffer(size_t elementCount = 1, const std::string& debugName = "") {
		return CreateBuffer(
				sizeof(T) * elementCount,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VMA_MEMORY_USAGE_GPU_ONLY,
				0,
				debugName.empty() ? "StorageBuffer" : debugName);
	}

	template <typename T>
	Buffer CreateStagingBuffer(size_t elementCount, const T* initialData = nullptr,
			const std::string& debugName = "") {
		Buffer buffer = CreateBuffer(
				sizeof(T) * elementCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VMA_MEMORY_USAGE_CPU_ONLY,
				VMA_ALLOCATION_CREATE_MAPPED_BIT,
				debugName.empty() ? "StagingBuffer" : debugName);

		if (initialData != nullptr && buffer.IsMapped()) {
			memcpy(buffer.GerMappedData(), initialData, sizeof(T) * elementCount);
		}

		return buffer;
	}

	template <typename T>
	Buffer CreateStagingBuffer(const std::vector<T>& data, const std::string& debugName = "") {
		return CreateStagingBuffer(data.size(), data.data(), debugName);
	}

	Image CreateImage2D(uint32_t width, uint32_t height, VkFormat format,
			VkImageUsageFlags usage, VmaMemoryUsage memoryUsage,
			const std::string& debugName = "");

	Image CreateImage3D(uint32_t width, uint32_t height, uint32_t depth, VkFormat format,
			VkImageUsageFlags usage, VmaMemoryUsage memoryUsage,
			const std::string& debugName = "");

	void* MapMemory(VmaAllocation allocation);
	void UnmapMemory(VmaAllocation allocation);

	void FlushMemory(VmaAllocation allocation, VkDeviceSize offset = 0,
			VkDeviceSize size = VK_WHOLE_SIZE);
	void InvalidateMemory(VmaAllocation allocation, VkDeviceSize offset = 0,
			VkDeviceSize size = VK_WHOLE_SIZE);

	void* MapBuffer(const Buffer& buffer);
	void UnmapBuffer(const Buffer& buffer);

	void DestroyBuffer(Buffer& buffer) noexcept;
	void DestroyImage(Image& image) noexcept;
	void DestroyAllResources() noexcept;

public:
	//Vma Pool
	VmaPool CreateMemoryPool(const VmaPoolCreateInfo& poolInfo);
	void DestroyMemoryPool(VmaPool pool) noexcept;

	VmaAllocator GetAllocator() const noexcept { return allocator_; }
	bool IsValid() const noexcept { return allocator_ != VK_NULL_HANDLE; }

	struct MemoryStats {
		size_t totalAllocatedBytes = 0;
		size_t totalAllocationCount = 0;
		size_t usedBytes = 0;
		size_t allocationCount = 0;
		std::vector<VmaBudget> budgets;
	};

	MemoryStats GetMemoryStats() const;
	void PrintDetailedStats(bool verbose = false) const;

	static constexpr VkBufferCreateInfo CreateBufferInfo(VkDeviceSize size, VkBufferUsageFlags usage) {
		return VkBufferCreateInfo{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			nullptr,
			0,
			size,
			usage,
			VK_SHARING_MODE_EXCLUSIVE,
			0, nullptr
		};
	}

private:
	void CreateVulkanAllocator(const AllocatorConfig& config);
	void Cleanup() noexcept;
	void SetDebugName(VmaAllocation allocation, const std::string& name);

public:
	VulkanAllocator(const VkInstance& instance, const VkDevice& device,
			const VkPhysicalDevice& physicaldevice, const AllocatorConfig& config = {});
	~VulkanAllocator();

	VulkanAllocator(const VulkanAllocator&) = delete;
	VulkanAllocator& operator=(const VulkanAllocator&) = delete;
	VulkanAllocator(VulkanAllocator&& other) noexcept;
	VulkanAllocator& operator=(VulkanAllocator&& other) noexcept;

private:
	const VkInstance& instance_;
	const VkDevice& device_;
	const VkPhysicalDevice& physical_device_;

private:
	VmaAllocator allocator_;
#ifdef VMA_DEBUG_TRACKING
	std::unordered_set<VmaAllocation> trackedAllocations_;
#endif
};

} //namespace Driver::Vulkan::Memory

#endif