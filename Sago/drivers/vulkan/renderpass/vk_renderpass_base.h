#ifndef SG_VULKAN_RENDERPASS_BASE_h
#define SG_VULKAN_RENDERPASS_BASE_h

#include "core/io/log/log.h"

namespace Driver::Vulkan {

template <typename ConcreteWindow>
	requires std::is_class_v<ConcreteWindow>
struct RenderPassBase {
protected:
	void CreateRenderPass() {
		LogInfo("[Vulkan][{0}] Create {0} RenderPass", ConcreteWindow::kClassName);
		static_cast<const ConcreteWindow*>(this)->CreateRenderPassImpl();
	}

};

} //namespace Driver::Vulkan

#endif