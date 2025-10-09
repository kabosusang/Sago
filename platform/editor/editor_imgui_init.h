#ifndef SG_EDITOR_IMGUI_INIT_H
#define SG_EDITOR_IMGUI_INIT_H
#include "window/window_sdl.h"
#include "drivers/vulkan/vk_context_data.h"
#include <memory>
#include <vector>

namespace Platform {
class AppWindow;

class EditorUI {
public:
	EditorUI(const AppWindow& window);
    ~EditorUI();
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
    void Shutdown();
private:
	const AppWindow& window_;
    VkDevice device_ = VK_NULL_HANDLE;
    VkRenderPass uirenderpass_;
    VkDescriptorPool descriptor_pool_;
    bool initialized_{false};
    std::shared_ptr<Driver::Vulkan::VulkanContextData> data_;
};

} //namespace Platform

#endif