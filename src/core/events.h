#pragma once

#include <functional>

namespace coffeepot
{
    enum class EventType
    {
        WindowClosed,
        WindowResized,
        Char
    };

    class Event
    {
    public:
        virtual EventType getType() const = 0;
        virtual const char *toString() const = 0;
    };

    class WindowClosedEvent : public Event
    {
    public:
        WindowClosedEvent() = default;
        virtual EventType getType() const override { return EventType::WindowClosed; }
        virtual const char *toString() const override { return "WindowClosedEvent"; }
    };
    
    class WindowResizedEvent : public Event
    {
    public:
        WindowResizedEvent(int32_t width, int32_t height) : m_Width(width), m_Height(height) {}
        virtual EventType getType() const override { return EventType::WindowResized; }
        virtual const char *toString() const override { return "WindowResizedEvent"; }

    public:
        int32_t m_Width;
        int32_t m_Height;
    };

    class CharEvent : public Event
    {
    public:
        CharEvent(u_int32_t keycode) : m_Keycode(keycode) {}
        virtual EventType getType() const override { return EventType::Char; }
        virtual const char *toString() const override { return "CharEvent"; }

    public:
        u_int32_t m_Keycode;
    };

    using EventCallbackFn = std::function<void(Event&)>;
}