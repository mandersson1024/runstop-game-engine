#include "pch.h"
#include "Input.h"
#include "../EngineAssert.h"

namespace Engine
{
    std::shared_ptr<Input> Input::instance_;

    void Input::Init()
    {
        ENGINE_ASSERT(!instance_, "IO already initialized");
        ENGINE_LOG("Initializing IO");
        instance_ = std::make_shared<Input>();
    }

    void Input::Shutdown()
    {
    }

    Input::Input()
    {
        keyboard_ = std::make_unique<DirectX::Keyboard>();
        mouse_ = std::make_unique<DirectX::Mouse>();
    }

    void Input::SetWindow(HWND hwnd)
    {
        GetMouse().SetWindow(hwnd);
    }


    bool Input::IsKeyDown(DirectX::Keyboard::Keys key)
    {
        return GetKeyboard().GetState().IsKeyDown(key);
    }

    bool Input::IsAnyShiftKeyDown()
    {
        return 
            IsKeyDown(DirectX::Keyboard::LeftShift) ||
            IsKeyDown(DirectX::Keyboard::RightShift);
    }

    bool Input::IsAnyCtrlKeyDown()
    {
        return
            IsKeyDown(DirectX::Keyboard::LeftControl) ||
            IsKeyDown(DirectX::Keyboard::RightControl);
    }

    bool Input::IsAnyAltKeyDown()
    {
        return
            IsKeyDown(DirectX::Keyboard::LeftAlt) ||
            IsKeyDown(DirectX::Keyboard::RightAlt);
    }

    bool Input::IsLeftMouseButtonDown()
    {
        return GetMouse().GetState().leftButton;
    }

    bool Input::IsRightMouseButtonDown()
    {
        return GetMouse().GetState().rightButton;
    }

    bool Input::IsAnyMouseButtonDown()
    {
        auto state = GetMouse().GetState();
        return state.leftButton || state.rightButton || state.middleButton || state.xButton1 || state.xButton2;
    }

    float Input::ConsumeScrollWheelValue()
    {
        // Here we assume that 
        float value = (float)GetMouse().GetState().scrollWheelValue / (float)WHEEL_DELTA;
        GetMouse().ResetScrollWheelValue();
        return value;
    }

}
