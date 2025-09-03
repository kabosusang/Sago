#include "vk_semaphore.h"

#include "core/io/log/log.h"

namespace Driver::Vulkan {

VulkanSemaphore::VulkanSemaphore(const VkDevice device) :
		device_(device) {
	createSemaphore();
}

VulkanSemaphore::~VulkanSemaphore() {
	if (semaphore_ != VK_NULL_HANDLE) {
		vkDestroySemaphore(device_, semaphore_, nullptr);
		semaphore_ = VK_NULL_HANDLE;
	}
}

VulkanSemaphore::VulkanSemaphore(VulkanSemaphore&& other) noexcept
		:
		device_(other.device_),
		semaphore_(other.semaphore_),
		value_(other.value_),
		isTimeline_(other.isTimeline_) {
	other.semaphore_ = VK_NULL_HANDLE;
}

VulkanSemaphore& VulkanSemaphore::operator=(VulkanSemaphore&& other) noexcept {
	if (this != &other) {
		if (semaphore_ != VK_NULL_HANDLE) {
			vkDestroySemaphore(device_, semaphore_, nullptr);
		}

		device_ = other.device_;
		semaphore_ = other.semaphore_;
		value_ = other.value_;
		isTimeline_ = other.isTimeline_;

		other.semaphore_ = VK_NULL_HANDLE;
	}
	return *this;
}


void VulkanSemaphore::createSemaphore() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &semaphore_) != VK_SUCCESS) {
        LogErrorDetail("[Vulkan] Failed to create semaphore!");
    }
}




} //namespace Driver::Vulkan