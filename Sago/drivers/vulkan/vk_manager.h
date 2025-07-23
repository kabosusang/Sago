#ifndef SG_VK_MANAGER_H
#define SG_VK_MANAGER_H


#include "window_interface.h"

namespace Driver::Vulkan {

class VkContextRendering{
public:

    VkContextRendering() = default;

    void Init();

private:
    void InitVulkanInstance();


private:
    Platform::AppWindow* window_;

};

} //namespace driver::vulkan

#endif