#include "pch.h"
#include "Application.h"
#include "LayerStack.h"
#include "Layer.h"
#include "RubikController.h"
#include "../Platform/Window.h"
#include "../Platform/Input.h"
#include "../ProjectSettings.h"
#include "../Scene/GameObjectController.h"
#include "../Utils/FrameTimer.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Graphics.h"
#include "../Platform/NativeWindow.h"
#include "../Assets/Assets.h"

namespace Engine {

    Application* Application::instance_;

    Application::Application()
        : projectSettings_(std::make_unique<ProjectSettings>("project.json"))
        , layerStack_(std::make_unique<LayerStack>())
    {
    }

    void Application::Init(HINSTANCE moduleHandle, int nCmdShow)
    {
        ENGINE_LOG("Initializing Application");

        instance_ = this;

        projectSettings_->Load();

        window_ = std::make_shared<Window>(moduleHandle, "RUNSTOP", 1280, 720);
        window_->SetEventCallback([this](Event& event) { OnEvent(event); });
        window_->Spawn(nCmdShow);

        Input::Init();

        {
            GraphicsSettings settings
            {
                .antiAliasingSampleCount = 8,
                .antiAliasingQuality = 0,
            };

            auto hwnd = window_->GetNativeWindow().GetHWnd();
            Graphics::Init(settings, hwnd);
        }

        Assets::Init();
        renderer_ = std::make_unique<Renderer>();

        {
            //
            // REGISTER CODE ASSETS (until we have scripting)
            //

            GameObjectController::GetLibrary().Register("rubik_controller", GameObjectController::GetFactory<CodeAssets::RubikController>());
        }

        ENGINE_LOG("Application is initialized");

        OnInitialized();
    }

    void Application::Shutdown()
    {
        while (layerStack_->begin() != layerStack_->end())
        {
            auto layer = *layerStack_->begin();
            layerStack_->PopLayer(layer);
            layer->OnDetach();
        }

        Assets::AwaitThreads();
    }

    Application::~Application()
    {
        ENGINE_LOG("Destroying Application");
    }

    void Application::OnEvent(Event& evt)
    {
        if (evt.GetEventType() == EventType::WindowActivated)
        {
            if (Graphics::IsInitialized())
            {
                Assets::ReloadDirtyAssets();
            }
        }

        if (evt.GetEventType() == EventType::WindowClose)
        {
            Stop();
            evt.handled = true;
        }

        if (evt.GetEventType() == EventType::KeyPress)
        {
            if (static_cast<KeyPressEvent&>(evt).GetKeyCode() == DirectX::Keyboard::Escape)
            {
                Stop();
                evt.handled = true;
            }
        }

        for (auto it = layerStack_->end(); it != layerStack_->begin(); )
        {
            --it;
            (*it)->OnEvent(evt);

            if (evt.handled)
            {
                break;
            }
        }
    }

    // The LayerStack will delete the layer when the Application is destroyed
    void Application::PushLayer(std::shared_ptr<Layer> layer)
    {
        ENGINE_LOG("Pushing layer: \"{}\"", layer->GetName());
        layerStack_->PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(std::shared_ptr<Layer> layer)
    {
        layerStack_->PopLayer(layer);
        layer->OnDetach();
    }

    void Application::Run()
    {
        FrameTimer timer;

        while (running_)
        {
            timer.Tick();

            renderer_->Clear();

            for (auto& layer : *layerStack_)
            {
                layer->OnUpdate(timer.GetDeltaTime());
            }			

            renderer_->Present();

            // We poll the events last, because this could result in
            // some disruptive stuff happening in WndProc (such as 
            // closing the window). We don't want that in the middle
            // of the loop.

            NativeWindow::PollEvents();
        }
    }

    void Application::Stop()
    {
        running_ = false;
    }

}
