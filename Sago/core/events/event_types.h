#ifndef SG_EVENT_TYPE_H
#define SG_EVENT_TYPE_H

#include <stdalign.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>
#include <variant>

#include "meta/meta.h"


//data
#include "drivers/vulkan/vk_context_data.h"

namespace Core::Event {

enum class MouseButton : char { Left,Right,Middle,X1,X2 };
enum class MouseButtonState : char { Pressed,Released };

template <typename EventVariant>
struct EventTypeMap;

template <typename... Events>
struct EventTypeMap<std::variant<Events...>> {
	static constexpr size_t count = sizeof...(Events);

	template <typename Event>
	static constexpr size_t index_of() {
		using namespace meta::type;
		return index_of_type_v<Event, Events...>;
	}

	template <size_t I>
	using type_at = std::tuple_element_t<I, std::tuple<Events...>>;
};

/**
 * @brief
 * Event Type
 */

struct WindowResizeEvent {
	int32_t width_;
	int32_t height_;
	uint32_t window_id_;
};

struct WindowMinimizeEvent {
	bool minimized_;
	uint32_t window_id_;
};

struct KeyEvent {
	uint32_t key_code_;
	bool pressed_;
	uint64_t timestamp_;
};

/**
 * @brief MouseEvent
 *
 */

struct MouseBaseEvent {
	uint32_t timestamp;
	//int32_t device_id;
};

struct MouseDeviceEvent : MouseBaseEvent {
	enum class EventType { Connected,
		Disconnected };
	EventType event_type;
};

struct MouseMotionEvent : MouseBaseEvent {
	int32_t x_, y_;
	int32_t rel_x, rel_y; // relate
	uint32_t button_state_; // if (motion.state & SDL_BUTTON_LMASK)
};

struct MouseButtonEvent : MouseBaseEvent {
	MouseButton button_;
	MouseButtonState state_;
	uint8_t click_count_;
	int32_t x_, y_;
};

struct MouseWheelEvent : MouseBaseEvent {
	float scroll_horizontal;
	float scroll_vertical;
	bool is_precise; //Is TouchPad
};

struct RendererDataInitEvent{
	std::shared_ptr<Driver::Vulkan::VulkanContextData> data_;
};




} //namespace Core::Event

#endif