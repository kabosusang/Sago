#ifndef SG_EVENT_TYPE_H
#define SG_EVENT_TYPE_H

#include <stdalign.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Core::Event {

struct alignas(32) BaseEvent {
	uint32_t type_id;
	uint64_t timestamp;
    alignas(16) uint8_t data[24];
    
    template<typename T>
    static consteval bool fits(){
        return sizeof(T) <= sizeof(data) && alignof(T) <= alignof(data);
    }
};


//Window
struct WindowResizeEvent {
    int32_t width;
    int32_t height;
    uint32_t window_id;
};

struct WindowMinimizeEvent {
    bool minimized;
    uint32_t window_id;
};

struct KeyEvent {
    uint32_t key_code;
    bool pressed;
    uint32_t timestamp;
};

struct MouseEvent {
    int32_t x, y;
    uint32_t button_mask;
    int32_t scroll_delta;
};

template <typename... Events>
class EventRegistry {
public:
    template <typename T>
    static constexpr bool is_registered() {
        return (std::is_same_v<T, Events> || ...);
    }
};

using SGEventTypes = EventRegistry<
    WindowResizeEvent,
    WindowMinimizeEvent, 
    KeyEvent,
    MouseEvent
>;

static_assert(BaseEvent::fits<WindowResizeEvent>(), "Event too large");
static_assert(BaseEvent::fits<WindowMinimizeEvent>(), "Event too large");
static_assert(BaseEvent::fits<KeyEvent>(), "Event too large");
static_assert(BaseEvent::fits<MouseEvent>(), "Event too large");

////////////////////////////////ID//////////////////////////////////////////
struct EventTypeIDs {
    static constexpr uint32_t WindowResize      = 0x0100;
    static constexpr uint32_t WindowMinimize    = 0x0101;
    static constexpr uint32_t WindowFocus       = 0x0102;
 
    static constexpr uint32_t KeyEvent          = 0x0200;
    static constexpr uint32_t MouseEvent        = 0x0201;
    static constexpr uint32_t GamepadEvent      = 0x0202;
    
    static constexpr uint32_t SwapchainRecreate = 0x0300;
    static constexpr uint32_t RenderFrame       = 0x0301;
    static constexpr uint32_t PreRender         = 0x0302;
    static constexpr uint32_t PostRender        = 0x0303;
    
    static constexpr uint32_t AppQuit           = 0x0400;
    static constexpr uint32_t AppSuspend        = 0x0401;
    
    static constexpr uint32_t UserEventStart    = 0x1000;
    static constexpr uint32_t UserEventEnd      = 0xFFFF;
};

template<typename T> struct EventTypeID;
template<> struct EventTypeID<WindowResizeEvent> {
    static constexpr uint32_t value_ = EventTypeIDs::WindowResize;
};

template<> struct EventTypeID<WindowMinimizeEvent> {
    static constexpr uint32_t value_ = EventTypeIDs::WindowMinimize;
};

template<> struct EventTypeID<KeyEvent> {
    static constexpr uint32_t value_ = EventTypeIDs::KeyEvent;
};

template<> struct EventTypeID<MouseEvent> {
    static constexpr uint32_t value_ = EventTypeIDs::MouseEvent;
};

static_assert(EventTypeID<WindowResizeEvent>::value_ != EventTypeID<WindowMinimizeEvent>::value_);









} //namespace Core

#endif