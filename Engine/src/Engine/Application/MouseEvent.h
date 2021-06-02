#pragma once
#include <sstream>
#include "Event.h"


namespace Engine {

    class MouseDownEvent : public Event
    {
    public:
        MouseDownEvent(int button)
            : m_button(button)
        {}

        EventType GetEventType() const override { return EventType::MouseDown; }
        inline bool IsMouseEvent() const override { return true; }
        int GetButton() const { return m_button; }

        const std::string ToString() const override
        {
            std::ostringstream ss;
            ss << "[MouseDownEvent: button=" << m_button << "]";
            return ss.str();
        }

    private:
        int m_button;
    };

    class MouseUpEvent : public Event
    {
    public:
        MouseUpEvent(int button)
            : m_button(button) 
        {}

        EventType GetEventType() const override { return EventType::MouseUp; }
        int GetButton() const { return m_button; }

        const std::string ToString() const override
        {
            std::ostringstream ss;
            ss << "[MouseUpEvent: button=" << m_button << "]";
            return ss.str();
        }

    private:
        int m_button;
    };

    class MouseMoveEvent : public Event
    {
    public:
        MouseMoveEvent(int xPos, int yPos)
            : m_xPos(xPos)
            , m_yPos(yPos)
        {}

        EventType GetEventType() const override { return EventType::MouseMove; }
        inline bool IsMouseEvent() const override { return true; }

        int GetXPos() const { return m_xPos; }
        int GetYPos() const { return m_yPos; }

        const std::string ToString() const override
        { 
            std::ostringstream ss;
            ss << "[MouseMoveEvent: position=(" << m_xPos << "," << m_yPos << ")" << "]";
            return ss.str();
        }

    private:
        int m_xPos;
        int m_yPos;
    };

    class MouseScrollEvent : public Event
    {
    public:
        MouseScrollEvent(float delta)
            : m_delta(delta)
        {}

        EventType GetEventType() const override { return EventType::MouseScroll; }
        inline bool IsMouseEvent() const override { return true; }

        float GetDelta() const { return m_delta; }

        const std::string ToString() const override
        {
            std::ostringstream ss;
            ss << "[MouseScrollEvent: delta=" << m_delta << "]";
            return ss.str();
        }

    private:
        float m_delta;
    };

}



