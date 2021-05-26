#pragma once

#include "Event.h"
#include <sstream>

namespace Engine {

	class KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
		inline bool IsKeyEvent() const override { return true; }

	protected:
		KeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	class KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(int keycode, int autoRepeat)
			: KeyEvent(keycode)
			, m_autoRepeat(autoRepeat)
		{}

		EventType GetEventType() const override { return EventType::KeyPress; }

		const std::string ToString() const override
		{
			std::ostringstream os;
			os << "[KeyPressEvent: keycode=" << m_KeyCode;

			if (m_autoRepeat)
			{
				os << " (auto repeat)";
			}

			os << "]";

			return os.str();
		}

	private:
		int m_autoRepeat;
	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(int charcode, int autoRepeat)
			: m_charCode(charcode)
			, m_autoRepeat(autoRepeat)
		{}

		inline int GetCharCode() const { return m_charCode; }
		EventType GetEventType() const override { return EventType::KeyTyped; }

		const std::string ToString() const override
		{
			std::ostringstream os;
			os << "[KeyTypedEvent: character code=" << m_charCode;

			if (m_autoRepeat)
			{
				os << "(auto repeat)";
			}
			
			os << "]";

			return os.str();
		}

	private:
		int m_charCode;
		int m_autoRepeat;
	};

	class KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(int keycode)
			: KeyEvent(keycode) {}

		EventType GetEventType() const override { return EventType::KeyRelease; }

		const std::string ToString() const override
		{
			std::ostringstream ss;
			ss << "[KeyReleaseEvent: keycode=" << m_KeyCode << "]";
			return ss.str();
		}
	};


}





