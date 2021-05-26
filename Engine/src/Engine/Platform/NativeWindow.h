#pragma once
#include <functional>
#include <bitset>
#include <windows.h>
#include "INativeWindowListener.h"

namespace Engine
{
	struct INativeWindowListener;

	class NativeWindow
	{
	public:
		NativeWindow(HINSTANCE moduleHandle, const std::string& title, int width, int height);
		~NativeWindow();

		static void PollEvents();

		void Spawn(INativeWindowListener* listener, int nCmdShow);
	
		void SetTitle(const char* title);

		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
	
		HWND GetHWnd() const { return hwnd_; }

		inline void SetWinProcHandler(std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> callback) { additionalWinProcHandler_ = callback; }
		void RemoveWinProcHandler() { additionalWinProcHandler_ = nullptr; }

	private:
		void RegisterClass();
		void UnregisterClass();
		
		static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

		inline bool TestAutoRepeatBit(LPARAM lParam) { return std::bitset<32>(lParam).test(30); }

	private:
		static NativeWindow* nativeWindow_;
		static constexpr const char* className_ = "NativeWindowClass";
		
		HINSTANCE moduleHandle_;
		HWND hwnd_;
		INativeWindowListener* listener_;
		std::string title_;
		int width_;
		int height_;
		bool inSizemove_;
		bool inSuspend_;
		bool minimized_;
		bool fullscreen_;

		// Inserted before our own WinProc handling. Typically used by ImGui.
		std::function<LRESULT(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> additionalWinProcHandler_;
	};
	
}
