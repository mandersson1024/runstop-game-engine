#include "pch.h"
#include "Window.h"
#include "NativeWindow.h"

namespace Engine 
{

    Window::Window(HINSTANCE moduleHandle, const std::string& title, uint32_t width, uint32_t height)
        : title_(title)
        , width_(width)
        , height_(height)
    {
        ENGINE_LOG("Creating window: \'{}\' ({},{})", title, width, height);
        nativeWindow_ = std::make_shared<NativeWindow>(moduleHandle, title, width, height);
    }

    void Window::Spawn(int nCmdShow)
    {
        nativeWindow_->Spawn(this, nCmdShow);
    }

    void Window::SetTitle(const std::string& title)
    {
        GetNativeWindow().SetTitle(title.c_str());
    }

    void Window::OnClose()
    {
        WindowCloseEvent evt;
        eventCallback_(evt);
    }

    void Window::OnMoved()
    {
        //ENGINE_LOG_DEBUG_WARNING("Not yet implemented");
    }

    void Window::OnSuspending()
    {
        ENGINE_LOG_DEBUG_WARNING("Not yet implemented");
    }

    void Window::OnResuming()
    {
        ENGINE_LOG_DEBUG_WARNING("Not yet implemented");
    }

    void Window::OnSizeChanged(int width, int height)
    {
        width_ = width;
        height_ = height;

        WindowResizeEvent evt(width, height);
        eventCallback_(evt);
    }

    void Window::OnActivated()
    {
        WindowActivatedEvent evt;
        eventCallback_(evt);
    }

    void Window::OnDeactivated()
    {
        ENGINE_LOG_DEBUG_WARNING("Not yet implemented");
    }

    void Window::OnKeyDown(int keycode, bool autoRepeat)
    {
        KeyPressEvent evt(keycode, autoRepeat);
        //SetTitle(evt.ToString().c_str());
        eventCallback_(evt);
    }

    void Window::OnKeyTyped(int keycode, bool autoRepeat)
    {
        KeyTypedEvent evt(keycode, autoRepeat);
        eventCallback_(evt);
    }

    void Window::OnKeyUp(int keycode)
    {
        KeyReleaseEvent evt(keycode);
        //SetTitle(evt.ToString());
        eventCallback_(evt);
    }

    void Window::OnMouseMove(int xPos, int yPos)
    {
        MouseMoveEvent evt(xPos, yPos);
        //SetTitle(evt.ToString());
        eventCallback_(evt);
    }

    void Window::OnMouseButtonDown(int button)
    {
        MouseDownEvent evt(button);
        //SetTitle(evt.ToString());
        eventCallback_(evt);
    }

    void Window::OnMouseButtonUp(int button)
    {
        MouseUpEvent evt(button);
        //SetTitle(evt.ToString());
        eventCallback_(evt);
    }

    void Window::OnMouseScroll(float delta)
    {
        MouseScrollEvent evt(delta);
        //SetTitle(evt.ToString());
        eventCallback_(evt);
    }

}
