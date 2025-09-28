#ifndef SG_EVENT_SYSTEM_H
#define SG_EVENT_SYSTEM_H
#include "common/single_internal.h"
#include "event_dispatcher.h"
#include <utility>

namespace Core::Event {
enum class ThreadCategory {
	Main,
	Renderer,
	Physical
};
class EventSystem : public Common::Singleton<EventSystem, Common::GlobalSingetonTag> {
	friend class Common::Singleton<EventSystem, Common::GlobalSingetonTag>;
	DEFINE_CLASS_SINGLTEN(EventSystem);

private:
	GameEventDispatcher main_dispatcher_;
	RendererEventDispatcher renderer_dispatcher_;
public:
	GameEventDispatcher& GetMainDispatcher(){return main_dispatcher_;}
	RendererEventDispatcher& GetRendererDispatcher(){return renderer_dispatcher_;}

public:
	//broad
	template <typename Event>
	void BroadAllCast(Event&& event) {
		publishEvent<ThreadCategory::Main, Event>(std::forward<Event>(event));
		publishEvent<ThreadCategory::Renderer, Event>(std::forward<Event>(event));
	}

	//Publish
	template <ThreadCategory Category= ThreadCategory::Main, typename Event>
	void PublishEvent(Event&& event) {
		if constexpr (Category == ThreadCategory::Main) {
			main_dispatcher_.publish(std::forward<Event>(event));
		}

		if constexpr (Category == ThreadCategory::Renderer) {
			renderer_dispatcher_.publish(std::forward<Event>(event));
		}
	}

	//Batch Publish
	template <ThreadCategory Category = ThreadCategory::Main, typename... Events>
	void PublishEventBulk(Events&&... events) {
		if constexpr (Category == ThreadCategory::Main) {
			main_dispatcher_.publishBulk(std::forward<Events>(events)...);
		}

		if constexpr (Category == ThreadCategory::Renderer) {
			renderer_dispatcher_.publishBulk(std::forward<Events>(events)...);
		}
	}

	//SDL
	template <ThreadCategory Category = ThreadCategory::Main>
	void PublishSDLEvent(const SDL_Event& sdl_event) {
		if constexpr (Category == ThreadCategory::Main) {
			main_dispatcher_.publishSDL(sdl_event);
		}

		if constexpr (Category == ThreadCategory::Renderer) {
			renderer_dispatcher_.publishSDL(sdl_event);
		}
	}

	//Process
	template <ThreadCategory Category= ThreadCategory::Main>
	void ProcessaAllEvent() {
		if constexpr (Category == ThreadCategory::Main) {
			main_dispatcher_.processAllEvents();
		}

		if constexpr (Category == ThreadCategory::Renderer) {
			renderer_dispatcher_.processAllEvents();
		}
	}

	template <ThreadCategory Category = ThreadCategory::Main>
	void ProcessUpToEvents(float maxprocess) {
		if constexpr (Category == ThreadCategory::Main) {
			main_dispatcher_.processUpTo(maxprocess);
		}

		if constexpr (Category == ThreadCategory::Renderer) {
			renderer_dispatcher_.processUpTo(maxprocess);
		}
	}

public:
};

} //namespace Core::Event

#endif