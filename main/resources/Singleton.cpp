#include "../runtime.h"
#include "context/engine_context.h"
#include "core/events/event_system.h"

using namespace Context;
using namespace Core::Event;

//Event System
EventSystem EventSystem::instance_;
//Runtime
Runtime Runtime::instance_;
//Engine
EngineContext EngineContext::instance_;