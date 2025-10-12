#ifndef SG_EDITOR_IMGUI_INIT_H
#define SG_EDITOR_IMGUI_INIT_H
#include "window/window_sdl.h"
#include <memory>

#include "drivers/vulkan/vk_context_data.h"
#include "drivers/vulkan/vk_descritpor.h"
#include "drivers/vulkan/renderpass/vk_renderpass_ui.h"
#include "drivers/vulkan/renderpass/vk_frambuffer.h"

namespace Platform {
class AppWindow;

class EditorUI {
public:
	EditorUI(const AppWindow& window);
    ~EditorUI();
    void Shutdown();
public:
	void Init();
	void Init_Imgui(std::shared_ptr<Driver::Vulkan::VulkanContextData>);
	void Quit();

public:
	void NewFrame();
    void Render(VkCommandBuffer commandBuffer, VkImageView targetImageView);
    void ProcessEvent(SDL_Event& event);
    bool IsInitialized(){return initialized_;}
private:
    VkRenderPass CreateTraditionalRenderPass(VkDevice device, VkFormat format);

private:
	const AppWindow& window_;
    VkDevice device_ = VK_NULL_HANDLE;
    bool initialized_{false};
    std::shared_ptr<Driver::Vulkan::VulkanContextData> data_;

private:
    std::unique_ptr<Driver::Vulkan::VulkanDescriptorPoolNoSet<128>> despool_;
    std::unique_ptr<Driver::Vulkan::VulkanUIRenderPass> renderpass_;
    std::unique_ptr<Driver::Vulkan::VulkanFrameBuffer> frambuffer_;
};

} //namespace Platform

#endif