#ifndef SG_VK_DEVICE_H
#define SG_VK_DEVICE_H

#include <volk.h>

namespace Driver::Vulkan {

class VulkanDevice {
    VulkanDevice();
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
	VulkanDevice& operator=(const VulkanDevice&) = delete;
	VulkanDevice(VulkanDevice&&) = delete;
	VulkanDevice& operator=(VulkanDevice&&) = delete;
private:
    void CreateLogicalDevice() const;
private:
    VkDevice device_;
};

} //namespace Driver::Vulkan

#endif