#ifndef SG_VULKAN_FENCE_H
#define SG_VULKAN_FENCE_H

#include <volk.h>
#include <memory>
#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan {

class VulkanFence {
public:
    VulkanFence(const VkDevice device, VkFenceCreateFlags flags = 0);
    ~VulkanFence();

    VulkanFence(const VulkanFence&) = delete;
    VulkanFence& operator=(const VulkanFence&) = delete;
    VulkanFence(VulkanFence&& other) noexcept;
    VulkanFence& operator=(VulkanFence&& other) noexcept;

    VkFence getHandle() const { return fence_; }
    operator VkFence() const { return fence_; }

    void wait(uint64_t timeout = UINT64_MAX) const;
    void reset();
    VkResult getStatus() const;

private:
    void createFence(VkFenceCreateFlags flags);

private:
    VkDevice device_{VK_NULL_HANDLE};
    VkFence fence_{VK_NULL_HANDLE};
};

} // namespace Driver::Vulkan

#endif