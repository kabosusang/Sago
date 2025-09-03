#ifndef SG_VULKAN_SEMAPHORE_H
#define SG_VULKAN_SEMAPHORE_H
#include <volk.h>

namespace Driver::Vulkan {

class VulkanSemaphore {
public:
	VulkanSemaphore(const VkDevice device);
	~VulkanSemaphore();

	VulkanSemaphore(const VulkanSemaphore&) = delete;
	VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;
	VulkanSemaphore(VulkanSemaphore&& other) noexcept;
	VulkanSemaphore& operator=(VulkanSemaphore&& other) noexcept;
	
	VkSemaphore getHandle() const { return semaphore_; }
	operator VkSemaphore() const { return semaphore_; }
	//Time Line
	uint64_t getValue() const { return value_; }
	void signal(uint64_t value);
	bool wait(uint64_t value, uint64_t timeout = UINT64_MAX);

private:
	void createSemaphore();

private:
	VkDevice device_{ VK_NULL_HANDLE };
	VkSemaphore semaphore_{ VK_NULL_HANDLE };
	uint64_t value_{ 0 }; //Time Line
	bool isTimeline_{ false };
};

} //namespace Driver::Vulkan

#endif