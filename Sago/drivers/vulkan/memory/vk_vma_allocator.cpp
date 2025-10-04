#include "vk_vma_allocator.h"

#include "drivers/vulkan/vk_log.h"


#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1 
#define VMA_VULKAN_VERSION 1004000
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Driver::Vulkan::Memory {

VulkanAllocator::VulkanAllocator(const VkInstance& instance, const VkDevice& device,
		const VkPhysicalDevice& physicaldevice, const AllocatorConfig& config) :
		instance_(instance), device_(device), physical_device_(physicaldevice) {
	CreateVulkanAllocator(config);
}

VulkanAllocator::~VulkanAllocator() {
	Cleanup();
}

VulkanAllocator::VulkanAllocator(VulkanAllocator&& other) noexcept
		:
		instance_(other.instance_), device_(other.device_), physical_device_(other.physical_device_), allocator_(other.allocator_) {
	other.allocator_ = VK_NULL_HANDLE;
}

VulkanAllocator& VulkanAllocator::operator=(VulkanAllocator&& other) noexcept {
	if (this != &other) {
		Cleanup();
		allocator_ = other.allocator_;
		other.allocator_ = VK_NULL_HANDLE;
	}
	return *this;
}

void VulkanAllocator::CreateVulkanAllocator(const AllocatorConfig& config) {
	VmaAllocatorCreateInfo allocator_info{};
	allocator_info.vulkanApiVersion = config.vulkan_apiVersion;
	allocator_info.physicalDevice = physical_device_;
	allocator_info.device = device_;
	allocator_info.instance = instance_;
	allocator_info.flags = config.flags;
	//Volk
	VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
    
    allocator_info.pVulkanFunctions = &vulkanFunctions;


	if (config.enable_memorybudget) {
		allocator_info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	}

	if (config.enable_bufferdevice_address) {
		allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	}
	auto result = vmaCreateAllocator(&allocator_info, &allocator_);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to Create VMA Allocator!", result);
	}
}

void VulkanAllocator::Cleanup() noexcept {
	if (allocator_ != VK_NULL_HANDLE) {
		vmaDestroyAllocator(allocator_);
		allocator_ = VK_NULL_HANDLE;
	}
}

VulkanAllocator::Buffer VulkanAllocator::CreateBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VmaMemoryUsage memoryUsage,
		VmaAllocationCreateFlags allocationFlags,
		const std::string& debugName) {
	VkBufferCreateInfo buffer_info = CreateBufferInfo(size, usage);
	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = memoryUsage;
	alloc_info.flags = allocationFlags;

	Buffer buffer;
	auto result = vmaCreateBuffer(allocator_, &buffer_info, &alloc_info,
			&buffer.buffer, &buffer.allocation, &buffer.allocation_info);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to Create Buffer!", result);
	}

	if (!debugName.empty()) {
		SetDebugName(buffer.allocation, debugName);
		buffer.debug_name = debugName;
	}

#ifdef VMA_DEBUG_TRACKING
	trackedAllocations_.insert(buffer.allocation);
#endif

	return buffer;
}

VulkanAllocator::Image VulkanAllocator::CreateImage2D(
		uint32_t width,
		uint32_t height,
		VkFormat format,
		VkImageUsageFlags usage,
		VmaMemoryUsage memoryUsage,
		const std::string& debugName) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memoryUsage;

	Image image;
	VkResult result = vmaCreateImage(allocator_, &imageInfo, &allocInfo,
			&image.image, &image.allocation, &image.allocation_info);

	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to Create Image!", result);
	}

	if (!debugName.empty()) {
		SetDebugName(image.allocation, debugName);
		image.debug_name = debugName;
	}

#ifdef VMA_DEBUG_TRACKING
	trackedAllocations_.insert(image.allocation);
#endif

	return image;
}

VulkanAllocator::Image VulkanAllocator::CreateImage3D(
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		VkFormat format,
		VkImageUsageFlags usage,
		VmaMemoryUsage memoryUsage,
		const std::string& debugName) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_3D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = depth;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memoryUsage;

	Image image;
	VkResult result = vmaCreateImage(allocator_, &imageInfo, &allocInfo,
			&image.image, &image.allocation, &image.allocation_info);

	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to Create Image3D!", result);
	}

	if (!debugName.empty()) {
		SetDebugName(image.allocation, debugName);
		image.debug_name = debugName;
	}

#ifdef VMA_DEBUG_TRACKING
	trackedAllocations_.insert(image.allocation);
#endif

	return image;
}

void* VulkanAllocator::MapMemory(VmaAllocation allocation) {
	void* mappedData = nullptr;
	VkResult result = vmaMapMemory(allocator_, allocation, &mappedData);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to MapMemory!", result);
	}
	return mappedData;
}

void VulkanAllocator::UnmapMemory(VmaAllocation allocation) {
	vmaUnmapMemory(allocator_, allocation);
}

void* VulkanAllocator::MapBuffer(const Buffer& buffer) {
	if (!buffer.IsValid()) {
		LogErrorDetail("[Vulkan][Allocate]: Cannot map invalid buffer");
	}
	return MapMemory(buffer.allocation);
}

void VulkanAllocator::UnmapBuffer(const Buffer& buffer) {
	if (buffer.IsValid()) {
		UnmapMemory(buffer.allocation);
	}
}

void VulkanAllocator::FlushMemory(VmaAllocation allocation, VkDeviceSize offset, VkDeviceSize size) {
	VkResult result = vmaFlushAllocation(allocator_, allocation, offset, size);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to vmaFlushAllocation!", result);
	}
}

void VulkanAllocator::InvalidateMemory(VmaAllocation allocation, VkDeviceSize offset, VkDeviceSize size) {
	VkResult result = vmaInvalidateAllocation(allocator_, allocation, offset, size);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to vmaInvalidateAllocation!", result);
	}
}

void VulkanAllocator::DestroyBuffer(Buffer& buffer) noexcept {
	if (buffer.IsValid()) {
#ifdef VMA_DEBUG_TRACKING
		trackedAllocations_.erase(buffer.allocation);
#endif

		vmaDestroyBuffer(allocator_, buffer.buffer, buffer.allocation);
		buffer.Reset();
	}
}

void VulkanAllocator::DestroyImage(Image& image) noexcept {
	if (image.IsValid()) {
#ifdef VMA_DEBUG_TRACKING
		trackedAllocations_.erase(image.allocation);
#endif

		vmaDestroyImage(allocator_, image.image, image.allocation);
		image.Reset();
	}
}

//VmaPool
VmaPool VulkanAllocator::CreateMemoryPool(const VmaPoolCreateInfo& poolInfo) {
	VmaPool pool = VK_NULL_HANDLE;
	VkResult result = vmaCreatePool(allocator_, &poolInfo, &pool);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Allocate]: Failed to CreateVmaPool!", result);
	}
	return pool;
}

void VulkanAllocator::DestroyMemoryPool(VmaPool pool) noexcept {
	if (pool != VK_NULL_HANDLE) {
		vmaDestroyPool(allocator_, pool);
	}
}

VulkanAllocator::MemoryStats VulkanAllocator::GetMemoryStats() const {
	MemoryStats stats = {};

	VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
	vmaGetHeapBudgets(allocator_, budgets);

	stats.budgets.assign(budgets, budgets + VK_MAX_MEMORY_HEAPS);

	for (const auto& budget : stats.budgets) {
		stats.totalAllocatedBytes += budget.usage;
		stats.usedBytes += budget.usage;
		stats.totalAllocationCount += budget.statistics.allocationCount;
		stats.allocationCount += budget.statistics.allocationCount;
	}

	return stats;
}

void VulkanAllocator::PrintDetailedStats(bool verbose) const {
	auto stats = GetMemoryStats();
	LogInfoDetail("=== VMA Memory Statistics ===");
	LogInfoDetail("Total Allocated: {} bytes", stats.totalAllocatedBytes);
	LogInfoDetail("Total Allocations: {}", stats.totalAllocationCount);

	if (verbose) {
		for (size_t i = 0; i < stats.budgets.size(); ++i) {
			const auto& budget = stats.budgets[i];
			if (budget.usage > 0) {
				LogInfoDetail("Heap {}: {} / {} bytes", i, budget.usage, budget.budget);
			}
		}
	}
	LogInfoDetail("=============================");
}

#if defined(NDEBUG)

#else
void VulkanAllocator::SetDebugName(VmaAllocation allocation, const std::string& name) {
	// VK_EXT_debug_utils
	vmaSetAllocationName(allocator_, allocation, name.c_str());
}
#endif

} //namespace Driver::Vulkan::Memory