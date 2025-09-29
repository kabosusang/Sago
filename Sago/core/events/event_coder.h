#ifndef SG_EVENT_CODER_H
#define SG_EVENT_CODER_H

#include "SDL3/SDL_events.h"
#include "event_types.h"
#include <functional>
#include <optional>
#include <unordered_map>

namespace Core::Event {

template <typename EventVariant>
class EventCoder {
private:
    using EventMap = EventTypeMap<EventVariant>;
    using EncoderFunc = std::function<std::optional<EventVariant>(const SDL_Event&)>;
    using EncoderTable = std::unordered_map<uint32_t, EncoderFunc>;

    static std::optional<EventVariant> encode_window_event(const SDL_WindowEvent& wevent) {
        switch (wevent.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                return WindowResizeEvent{ wevent.data1, wevent.data2, wevent.windowID };
            case SDL_EVENT_WINDOW_MINIMIZED:
                return WindowMinimizeEvent{ true, wevent.windowID };
            case SDL_EVENT_WINDOW_RESTORED:
                return WindowMinimizeEvent{ false, wevent.windowID };
            default:
                return std::nullopt;
        }
    }

    static std::optional<EventVariant> encode_key_event(const SDL_KeyboardEvent& kevent) {
        switch (kevent.type) {
            case SDL_EVENT_KEY_DOWN:
                return KeyEvent{ kevent.key, true, kevent.timestamp };
            case SDL_EVENT_KEY_UP:
                return KeyEvent{ kevent.key, false, kevent.timestamp };
            default:
                return std::nullopt;
        }
    }

    static std::optional<EventVariant> encode_mouse_motion(const SDL_MouseMotionEvent& motion) {
        MouseMotionEvent event{};
        event.timestamp = motion.timestamp;
        event.x_ = motion.x;
        event.y_ = motion.y;
        event.rel_x = motion.xrel;
        event.rel_y = motion.yrel;
        event.button_state_ = motion.state;
        return event;
    }

    static std::optional<EventVariant> encode_mouse_button(const SDL_MouseButtonEvent& button) {
        MouseButtonEvent event{};
        event.timestamp = button.timestamp;
        event.x_ = button.x;
        event.y_ = button.y;
        event.state_ = (button.down) ? MouseButtonState::Pressed : MouseButtonState::Released;
        event.click_count_ = button.clicks;
        switch (button.button) {
            case SDL_BUTTON_LEFT:
                event.button_ = MouseButton::Left;
                break;
            case SDL_BUTTON_MIDDLE:
                event.button_ = MouseButton::Middle;
                break;
            case SDL_BUTTON_RIGHT:
                event.button_ = MouseButton::Right;
                break;
            case SDL_BUTTON_X1:
                event.button_ = MouseButton::X1;
                break;
            case SDL_BUTTON_X2:
                event.button_ = MouseButton::X2;
                break;
        }
        return event;
    }

public:
    static std::optional<EventVariant> encode(const SDL_Event& sdl_event) {
        static const EncoderTable encoder_table = build_encoder_table();
        
        auto it = encoder_table.find(sdl_event.type);
        if (it != encoder_table.end()) {
            return it->second(sdl_event);
        }
        return std::nullopt;
    }

private:
    static EncoderTable build_encoder_table() {
        EncoderTable table;

        table[SDL_EVENT_WINDOW_RESIZED] = table[SDL_EVENT_WINDOW_MINIMIZED] =
                table[SDL_EVENT_WINDOW_RESTORED] = [](const SDL_Event& e) {
                    return encode_window_event(e.window);
                };

        table[SDL_EVENT_KEY_DOWN] = table[SDL_EVENT_KEY_UP] = [](const SDL_Event& e) {
            return encode_key_event(e.key);
        };

        table[SDL_EVENT_MOUSE_MOTION] = [](const SDL_Event& e) {
            return encode_mouse_motion(e.motion);
        };

        table[SDL_EVENT_MOUSE_BUTTON_DOWN] = table[SDL_EVENT_MOUSE_BUTTON_UP] = [](const SDL_Event& e) {
            return encode_mouse_button(e.button);
        };

        table[SDL_EVENT_QUIT] = [](const SDL_Event& e) {
            return std::nullopt;
        };

        table[SDL_EVENT_MOUSE_WHEEL] = [](const SDL_Event& e) {
            return std::nullopt;
        };

        

        return table;
    }
};

} // namespace Core::Event

#endif