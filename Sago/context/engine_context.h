#ifndef SG_CONXTEX_H
#define SG_CONXTEX_H
#include <memory>


#include "common/single_internal.h"
#include "window_interface.h"

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
};



} //namespace context

#endif