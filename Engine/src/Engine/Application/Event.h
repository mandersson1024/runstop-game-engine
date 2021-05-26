#pragma once

#include <iostream>
#include <string>

namespace Engine {

	enum class EventType
	{
		None = 0,
		
		WindowClose, 
		WindowResize, 
		WindowActivated, 
		
		KeyPress, 
		KeyRelease,
		KeyTyped,
		
		MouseDown, 
		MouseUp, 
		MouseMove, 
		MouseScroll,
	};

	class Event
	{
	public:
		bool handled = false;
		
		virtual EventType GetEventType() const = 0;
		virtual const std::string ToString() const = 0;

		virtual bool IsMouseEvent() const { return false; }
		virtual bool IsKeyEvent() const { return false; }
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}
