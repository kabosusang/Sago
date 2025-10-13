#ifndef SG_EDITOR_IMGUI_INIT_H
#define SG_EDITOR_IMGUI_INIT_H
#include "window/window_sdl.h"
#include <memory>

#include "drivers/vulkan/vk_context_data.h"
#include "drivers/vulkan/vk_descritpor.h"
#include "drivers/vulkan/renderpass/vk_renderpass_ui.h"
#include "drivers/vulkan/renderpass/vk_frambuffer.h"
#include "drivers/vulkan/commands/vk_commandpool.h"


namespace Platform {
class AppWindow;
struct ImDrawData;

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
    void BuildUI();
    void ProcessEvent(const SDL_Event& event);
    bool IsInitialized(){return initialized_;}
public:
    VkCommandBuffer GetCurrentCommandBuffer() const {
        return command_buffers_[current_frame_];
    }
    void RecordRenderCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void RecreateFrameBuffer(const std::vector<VkImageView>& imageview,VkExtent2D extent);
private:
	const AppWindow& window_;
    VkDevice device_ = VK_NULL_HANDLE;
    bool initialized_{false};
    std::shared_ptr<Driver::Vulkan::VulkanContextData> data_;

private:
    std::unique_ptr<Driver::Vulkan::VulkanDescriptorPoolNoSet<128>> despool_;
    std::unique_ptr<Driver::Vulkan::VulkanUIRenderPass> renderpass_;
    std::unique_ptr<Driver::Vulkan::VulkanFrameBuffer> frambuffer_;
private:
    void CreateCommandBuffers();
    void CreateSyncObjects();
    
    std::unique_ptr<Driver::Vulkan::VulkanCommandPool> command_pool_;
    std::vector<VkCommandBuffer> command_buffers_;
    std::vector<VkFence> in_flight_fences_;
    std::vector<VkSemaphore> image_available_semaphores_;
    std::vector<VkSemaphore> render_finished_semaphores_;
    uint32_t current_frame_ = 0;
    uint32_t max_frames_in_flight_ = 2;

private:
    mutable std::mutex frame_mutex_;
    bool frame_ready_{false};
};

} //namespace Platform

#endif