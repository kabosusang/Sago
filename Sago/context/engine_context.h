#ifndef SG_ENGINE_CONXTEX_H
#define SG_ENGINE_CONXTEX_H
#include <memory>
#include <mutex>

#include "context_base.h"
#include "meta/traits/class_traits.h"
//Controller
#include "controller/framerate_controller.h"
//platform
#include "window_interface.h"
//engine
#include "common/single_internal.h"
//Renderer Context
#include "renderer/renderer_context.h"

namespace Context {

class EngineContext : public Common::Singleton<EngineContext, Common::GlobalSingetonTag>, public ContextBase<EngineContext> {
	friend class Common::Singleton<EngineContext, Common::GlobalSingetonTag>;
	friend class ContextBase<EngineContext>;
	DEFINE_CLASS_NAME(EngineContext);
	DEFINE_CLASS_SINGLTEN(EngineContext);

public:
	void InitImpl();
	void Tick();
	void Quit();

	EngineContext();
	~EngineContext();

private:
	void ListenEventImpl();

public:
	void StartFrame(){fps_controller_.StartFrame();}
	void EndFrame(){fps_controller_.EndFrame();}

private:
	std::unique_ptr<Platform::AppWindow> window_;
	Controller::FrameRateController fps_controller_{ 144 };

private: //Pause Engine
	std::atomic<bool> is_paused_{ false };

private:
	//All Thread Class
	std::unique_ptr<Renderer::RendererContext> renderer_;
};

} //namespace Context

#endif