#pragma once
#include "DirectXTK/Keyboard.h"
#include "DirectXTK/Mouse.h"

namespace Engine
{
	class Input
	{
	public:
        Input();

		static void Init();
		static void Shutdown();

		static std::shared_ptr<Input> Get() { return instance_; }

		static void SetWindow(HWND hwnd);

		static bool IsKeyDown(DirectX::Keyboard::Keys key);
		static bool IsAnyShiftKeyDown();
		static bool IsAnyCtrlKeyDown();
		static bool IsAnyAltKeyDown();

		static DirectX::Keyboard& GetKeyboard() { return *instance_->keyboard_; }
		static DirectX::Mouse& GetMouse() { return *instance_->mouse_; }


        static bool IsLeftMouseButtonDown();
        static bool IsRightMouseButtonDown();
		static bool IsAnyMouseButtonDown();

		static float ConsumeScrollWheelValue();

	private:
		static std::shared_ptr<Input> instance_;
		std::unique_ptr<DirectX::Keyboard> keyboard_;
		std::unique_ptr<DirectX::Mouse> mouse_;

	};

}