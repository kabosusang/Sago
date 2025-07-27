#ifndef SG_VULKAN_LOG_h
#define SG_VULKAN_LOG_h
#include "core/io/log/log.h"
#include <vulkan/vk_enum_string_helper.h>


namespace Driver::Vulkan {

#define VK_LOG_ERROR(str, result) \
	LogErrorDetaill("{} : {}", str, string_VkResult(result))

} //namespace Driver::Vulkan

#endif