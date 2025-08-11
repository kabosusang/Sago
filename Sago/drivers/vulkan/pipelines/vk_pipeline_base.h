#ifndef SG_VK_BASE_PIPELINE_H
#define SG_VK_BASE_PIPELINE_H

#include <type_traits>
#include <volk.h>

#include "core/io/log/log.h"

namespace Driver::Vulkan {
template <typename ConcreteWindow>
    requires std::is_class_v<ConcreteWindow>
struct VulkanPipelineBase {
protected:
    void CreatePipeline(){
        static_cast<const ConcreteWindow*>(this)->CreatePipelineImpl();
    }


};

} //namespace Driver::Vulkan

#endif