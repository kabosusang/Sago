#ifndef SG_RENDERER_CONTEXT_H
#define SG_RENDERER_CONTEXT_H

#include <functional>

//Controller
#include "context/controller/framerate_controller.h"
//platform
#include "window/window_sdl.h"
#include "window_interface.h"
//Vulkan
#include "vulkan_context.h"


namespace Context::Renderer {

class RendererContext {
public:
    using callable = std::function<void()>;

    RendererContext(const Platform::AppWindow&,const Controller::FrameRateController&);

    RendererContext(const RendererContext&) = delete;
    RendererContext(const RendererContext&&) = delete;
    RendererContext& operator= (const RendererContext&) = delete;
    RendererContext& operator= (const RendererContext&&)  = delete;
private:
    void Init();
    void Tick() noexcept;

private:
    const Platform::AppWindow& window_;
    const Controller::FrameRateController& fpscontroller_;
private:
    std::jthread thread_;
    std::unique_ptr<VulkanContext> vk_context_;

};

} //namespace Context::Renderer

#endif