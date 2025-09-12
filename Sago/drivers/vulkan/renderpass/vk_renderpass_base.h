#ifndef SG_VULKAN_RENDERPASS_BASE_h
#define SG_VULKAN_RENDERPASS_BASE_h
#include <volk.h>

#include "core/io/log/log.h"

namespace Driver::Vulkan {

template <typename ConcreteWindow>
	requires std::is_class_v<ConcreteWindow>
struct RenderPassBase {
public:
	VkRenderPass GetRenderPass() const { return renderpass_; }
	operator VkRenderPass() const { return GetRenderPass(); }
protected:
	void CreateRenderPass() {
		LogInfo("[Vulkan][{0}] Create {0} RenderPass", ConcreteWindow::kClassName);
		static_cast<ConcreteWindow*>(this)->CreateRenderPassImpl();
	}

protected:
	VkRenderPass renderpass_;
};

} //namespace Driver::Vulkan

#endif