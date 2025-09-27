#ifndef SG_EVENT_CODEC_H
#define SG_EVENT_CODEC_H
#include "SDL3/SDL_events.h"

#include "core/memory/lockfree/MPMC/continuous_memorypool.h"
#include "core/time/time_utility.h"
#include "event_types.h"

#include <chrono>
#include <cstdint>

namespace Core::Event {

template <typename Type>
concept EventType = SGEventTypes::is_registered<Type>();
template <typename Type>
class EventEncoder {
public:
	static constexpr uint32_t TYPE_ID = EventTypeID<Type>::value_;
	using Pool = Core::Memory::ContinuousMemoryPool<BaseEvent, 4096>;
	using Time = Core::Time::TimeUtility;

	static BaseEvent* encode(Pool& pool, Type&& event) noexcept {
		BaseEvent* base = pool.allocate();
		if (!base) {
			return nullptr;
		}
		base->type_id = TYPE_ID;
		base->timestamp = Time::GetUtcTimestamp();

		if constexpr (sizeof(Type) <= sizeof(base->data)) {
			std::memcpy(base->data, &event, sizeof(Type));
		}

		return base;
	}

	static const Type* decode(const BaseEvent* base) noexcept {
		if (base->type_id == TYPE_ID) {
			return reinterpret_cast<const Type*>(base->data);
		}
		return nullptr;
	}
};

template <>
class EventEncoder<SDL_Event> {
	using Pool = Core::Memory::ContinuousMemoryPool<BaseEvent, 4096>;

public:
	static BaseEvent* encode(Pool& pool,
			const SDL_Event& event) noexcept {
		if (event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST) {
			return encodeWindowEvent(pool, event);
		}

		switch (event.type) {
			// case SDL_WINDOWEVENT:
			// 	return encodeWindowEvent(pool, sdl_event.window);
			// case SDL_KEYDOWN:
			// case SDL_KEYUP:
			// 	return encodeKeyEvent(pool, sdl_event.key);
			// case SDL_MOUSEMOTION:
			// case SDL_MOUSEBUTTONDOWN:
			// case SDL_MOUSEBUTTONUP:
			// 	return encodeMouseEvent(pool, sdl_event);
			// default:
			// 	return nullptr;
		}
	}

private:
	static BaseEvent* encodeWindowEvent(Pool& pool, const SDL_Event& wevent) {
		switch (wevent.type) {
			case SDL_EVENT_WINDOW_RESIZED: {
				WindowResizeEvent event{ wevent.window.data1, wevent.window.data2, wevent.window.windowID };
				return EventEncoder<WindowResizeEvent>::encode(pool, std::move(event));
			}
			case SDL_EVENT_WINDOW_MINIMIZED: {
				WindowMinimizeEvent event{ true, wevent.window.windowID };
				return EventEncoder<WindowMinimizeEvent>::encode(pool, std::move(event));
			}
			case SDL_EVENT_WINDOW_RESTORED: {
				WindowMinimizeEvent event{ false, wevent.window.windowID };
				return EventEncoder<WindowMinimizeEvent>::encode(pool, std::move(event));
			}
			default:
				return nullptr;
		}
	}

	// static BaseEvent* encodeKeyEvent(ContinuousMemoryPool<BaseEvent, 4096>& pool,
	// 		const SDL_KeyboardEvent& kevent) {
	// 	KeyEvent event{ kevent.keysym.sym, kevent.type == SDL_KEYDOWN, kevent.timestamp };
	// 	return EventEncoder<KeyEvent>::encode(pool, event);
	// }

	// static BaseEvent* encodeMouseEvent(ContinuousMemoryPool<BaseEvent, 4096>& pool,
	// 		const SDL_Event& sdl_event) {
	// 	MouseEvent event{};
	// 	if (sdl_event.type == SDL_MOUSEMOTION) {
	// 		event.x = sdl_event.motion.x;
	// 		event.y = sdl_event.motion.y;
	// 	} else {
	// 		event.x = sdl_event.button.x;
	// 		event.y = sdl_event.button.y;
	// 		event.button_mask = SDL_BUTTON(sdl_event.button.button);
	// 	}
	// 	return EventEncoder<MouseEvent>::encode(pool, event);
	// }
};

} //namespace Core::Event

#endif