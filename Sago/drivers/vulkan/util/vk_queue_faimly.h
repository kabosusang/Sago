#ifndef SG_VK_QUEUE_FAIMLY_H
#define SG_VK_QUEUE_FAIMLY_H
#include <cstdint>
#include <optional>
#include <variant>

#include <volk.h>

namespace Driver::Vulkan {

struct QueueFamilyIndice {
	std::optional<uint32_t> family_;
	inline bool isComplete() const {
		return family_.has_value();
	}
};

struct Graphy {
	auto operator()(const VkPhysicalDevice&) const;
};

struct Presente {
	auto operator()(const VkPhysicalDevice&,const VkSurfaceKHR&) const;
};

using QueueFamilyStrategy = std::variant<Graphy,Presente>;


QueueFamilyIndice FindIndice(const QueueFamilyStrategy&, 
	const VkPhysicalDevice& device = nullptr,const VkSurfaceKHR& surface = nullptr);


} //namespace Driver::Vulkan

#endif