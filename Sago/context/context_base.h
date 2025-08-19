#ifndef SG_CONTEXT_BASE_H
#define SG_CONTEXT_BASE_H

#include <type_traits>

#include "core/io/log/log.h"
namespace Context {

template <typename ConcreteWindow>
	requires std::is_class_v<ConcreteWindow>
struct ContextBase {
public:
	void Init() {
        LogInfo("[Context][{0}]: Current Thread Id: {1}",ConcreteWindow::kClassName,std::hash<std::thread::id>{}(std::this_thread::get_id()));
        static_cast<ConcreteWindow*>(this)->InitImpl();
	}
};



} //namespace Context

#endif