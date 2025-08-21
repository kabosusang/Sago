#ifndef SG_VK_BASE_PIPELINE_H
#define SG_VK_BASE_PIPELINE_H

#include <volk.h>
#include <string_view>
#include <type_traits>

#include "core/io/log/log.h"

namespace Driver::Vulkan {

template <typename ConcreteWindow>
	requires std::is_class_v<ConcreteWindow>
struct VulkanPipelineBase {
protected:
	void CreatePipeline() {
		LogInfo("[Vulkan][{0}] Create {0} Pipeline",ConcreteWindow::kClassName);
		static_cast<ConcreteWindow*>(this)->CreatePipelineImpl();
	}
	
	void HotReloadPipeline(std::string_view){

	}
};

} //namespace Driver::Vulkan

#endif