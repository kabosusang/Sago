#ifndef SG_ENGINE_CONXTEX_H
#define SG_ENGINE_CONXTEX_H
#include <memory>

//Controller
#include "controller/framerate_controller.h"
//platform
#include "window_interface.h"
//engine
#include "common/single_internal.h"
//Renderer Context
#include "renderer/renderer_context.h"


namespace Context {

class EngineContext : public Common::Singleton<EngineContext> {
	friend class Common::Singleton<EngineContext>;
public:
	void Init();
	void Tick();
	void Quit();
	
	EngineContext();
	~EngineContext();

private:
	std::unique_ptr<Platform::AppWindow> window_;
	Controller::FrameRateController fps_controller_{120};


private:
	//All Thread Class
	std::unique_ptr<Renderer::RendererContext> renderer_; 

};



} //namespace context

#endif