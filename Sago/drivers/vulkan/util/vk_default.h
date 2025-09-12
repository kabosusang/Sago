#ifndef SG_VULKAN_DEGAULT_H
#define SG_VULKAN_DEGAULT_H

#include <volk.h>

namespace Driver::Vulkan::Default::Colors {
inline constexpr VkClearValue Black = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
inline constexpr VkClearValue White = { { { 1.0f, 1.0f, 1.0f, 1.0f } } };
} //namespace Driver::Vulkan::Default::Colors

namespace Driver::Vulkan::Default::DepthStencil {
inline constexpr VkClearValue DepthOneStencilZero = { .depthStencil = { 1.0f, 0 } };
}

#endif