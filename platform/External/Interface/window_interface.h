#ifndef SG_INTERFACE_WINDOW_H
#define SG_INTERFACE_WINDOW_H

#include <type_traits>

#include "External/AsyncLog.h"

namespace Platform {
struct AppWindow;

template <typename ConcreteWindow>
	requires std::is_class_v<ConcreteWindow>
struct WindowInterface {
public:
	bool ShouldExit() const {
		return static_cast<const ConcreteWindow*>(this)->shouldexit_;
	}
	void Quit() const {
		static_cast<const ConcreteWindow*>(this)->QuitImpl();
		LogInfo("[Window][Quit] Success");
	}

	~WindowInterface() = default;
protected:
	WindowInterface() = default;
};

} //namespace Platform

#endif