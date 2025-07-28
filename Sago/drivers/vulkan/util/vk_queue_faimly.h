#ifndef SG_VK_QUEUE_FAIMLY_H
#define SG_VK_QUEUE_FAIMLY_H
#include <optional>

#include <volk.h>

namespace Driver::Vulkan {

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicis_family_;
	inline bool isComplete() const {
		return graphicis_family_.has_value();
	}
};

QueueFamilyIndices FindQueueFamilise(const VkPhysicalDevice& device);


} //namespace Driver::Vulkan

#endif