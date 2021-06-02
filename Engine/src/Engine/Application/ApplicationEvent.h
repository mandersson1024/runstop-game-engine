#pragma once

#include "Event.h"
#include <string>
#include <sstream>

namespace Engine {

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height) {}

        inline uint32_t GetWidth() const { return m_Width; }
        inline uint32_t GetHeight() const { return m_Height; }

        EventType GetEventType() const override { return EventType::WindowResize; }

        const std::string ToString() const override
        {
            std::ostringstream ss;
            ss << "[WindowResizeEvent: width=" << m_Width << ", height=" << m_Height << "]";
            return ss.str();
        }

    private:
        uint32_t m_Width;
        uint32_t m_Height;
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() {}

        EventType GetEventType() const override { return EventType::WindowClose; }

        const std::string ToString() const override
        {
            std::ostringstream ss;
            ss << "[WindowCloseEvent]";
            return ss.str();
        }
    };

    class WindowActivatedEvent : public Event
    {
    public:
        WindowActivatedEvent() {}

        EventType GetEventType() const override { return EventType::WindowActivated; }

        const std::string ToString() const override
        {
            std::ostringstream ss;
            ss << "[WindowActivatedEvent]";
            return ss.str();
        }
    };

}

