#include "vk_fence.h"
#include "core/io/log/log.h"

namespace Driver::Vulkan {

VulkanFence::VulkanFence(const VkDevice device, VkFenceCreateFlags flags) 
    : device_(device) {
    createFence(flags);
}

VulkanFence::~VulkanFence() {
    if (fence_ != VK_NULL_HANDLE) {
        vkDestroyFence(device_, fence_, nullptr);
        fence_ = VK_NULL_HANDLE;
    }
}

VulkanFence::VulkanFence(VulkanFence&& other) noexcept
    : device_(other.device_),
      fence_(other.fence_) {
    other.fence_ = VK_NULL_HANDLE;
}

VulkanFence& VulkanFence::operator=(VulkanFence&& other) noexcept {
    if (this != &other) {
        if (fence_ != VK_NULL_HANDLE) {
            vkDestroyFence(device_, fence_, nullptr);
        }
        
        device_ = other.device_;
        fence_ = other.fence_;
        
        other.fence_ = VK_NULL_HANDLE;
    }
    return *this;
}

void VulkanFence::createFence(VkFenceCreateFlags flags) {
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flags;
    
    if (vkCreateFence(device_, &fenceInfo, nullptr, &fence_) != VK_SUCCESS) {
        LogErrorDetail("[Vulkan] Failed to create fence!");
    }
}

void VulkanFence::wait(uint64_t timeout) const {
    vkWaitForFences(device_, 1, &fence_, VK_TRUE, timeout);
}

void VulkanFence::reset() {
    vkResetFences(device_, 1, &fence_);
}

VkResult VulkanFence::getStatus() const {
    return vkGetFenceStatus(device_, fence_);
}

} // namespace Driver::Vulkan