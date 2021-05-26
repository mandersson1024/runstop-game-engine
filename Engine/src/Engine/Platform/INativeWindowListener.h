#pragma once

namespace Engine
{

	struct INativeWindowListener
	{
		virtual void OnClose() = 0;
		virtual void OnMoved() = 0;
		virtual void OnSuspending() = 0;
		virtual void OnResuming() = 0;
		virtual void OnSizeChanged(int width, int height) = 0;
		virtual void OnActivated() = 0;
		virtual void OnDeactivated() = 0;
		virtual void OnKeyDown(int keycode, bool autoRepeat) = 0;
		virtual void OnKeyTyped(int keycode, bool autoRepeat) = 0;
		virtual void OnKeyUp(int keycode) = 0;
		virtual void OnMouseMove(int xPos, int yPos) = 0;
		virtual void OnMouseButtonDown(int button) = 0;
		virtual void OnMouseButtonUp(int button) = 0;
		virtual void OnMouseScroll(float delta) = 0;

	protected:
		~INativeWindowListener() = default;

	};

}