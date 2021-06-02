#pragma once
#include <memory>
#include <windows.h>
#include "Event.h"
#include "../Renderer/GraphicsSettings.h"

namespace Engine {

    class Window;
    class Renderer;
    class LayerStack;
    class Layer;
    class ProjectSettings;

    class Application
    {
    public:

        Application();
        virtual ~Application();
        
        static Application& Get() { return *instance_;  }
        static Renderer& GetRenderer() { return *instance_->renderer_; }
        static ProjectSettings& GetProjectSettings() { return *instance_->projectSettings_; }
        static Window& GetWindow() { return *instance_->window_; }

        void Init(HINSTANCE moduleHandle, int nCmdShow);
        void Shutdown();
        
        virtual void OnInitialized() = 0;

        void Run();
        void Stop();
        
        void PushLayer(std::shared_ptr<Layer> layer);
        void PopLayer(std::shared_ptr<Layer> layer);

    private:
        void OnEvent(Event& evt);

    private:
        static Application* instance_;
        
        std::unique_ptr<ProjectSettings> projectSettings_;
        std::shared_ptr<Window> window_;
        std::unique_ptr<LayerStack> layerStack_;
        std::unique_ptr<Renderer> renderer_;

        bool running_ = true;
    };

}
