#pragma once
#include <memory>
#include <functional>
#include <windows.h>
#include "INativeWindowListener.h"
#include "../Application/Event.h"

namespace Engine {

	class NativeWindow;

	class Window : public INativeWindowListener
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(HINSTANCE moduleHandle, const std::string& title, uint32_t width, uint32_t height);

		void Spawn(int nCmdShow);

		const std::string& GetTitle() const { return title_; }
		uint32_t GetWidth() const { return width_; }
		uint32_t GetHeight() const { return height_; }

		void SetTitle(const std::string& title);
		void SetWidth(uint32_t width) { width_ = width; }
		void SetHeight(uint32_t height) { height_ = height; }

		void SetEventCallback(const EventCallbackFn& callback) { eventCallback_ = callback; }
		NativeWindow& GetNativeWindow() { return *nativeWindow_; }

		void OnClose() override;
		void OnMoved() override;
		void OnSuspending() override;
		void OnResuming() override;
		void OnSizeChanged(int width, int height) override;
		void OnActivated() override;
		void OnDeactivated() override;
		void OnKeyDown(int keycode, bool autoRepeat) override;
		void OnKeyTyped(int keycode, bool autoRepeat) override;
		void OnKeyUp(int keycode) override;
		void OnMouseMove(int xPos, int yPos) override;
		void OnMouseButtonDown(int button) override;
		void OnMouseButtonUp(int button) override;
		void OnMouseScroll(float delta) override;

	private:
		std::shared_ptr<NativeWindow> nativeWindow_;
		EventCallbackFn eventCallback_;
		std::string title_;
		uint32_t width_;
		uint32_t height_;
	};

}

