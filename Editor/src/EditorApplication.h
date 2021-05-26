#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Engine/Application/Application.h"
#include "Engine/Raycast/Raycast.h"

namespace Engine
{
    class SceneAsset;
    class Scene;
}

namespace Editor
{

    class EditorCameraController;
    class SceneLayer;
    class GizmoLayer;
    class ImGuiLayer;

    enum class SkyboxDisplayType
    {
        Skybox = 0,
        Irradiance = 1,
        Radiance = 2,
    };

    class EditorApplication : public Engine::Application
    {
    public:
        EditorApplication();

        static EditorApplication& Get() { return static_cast<EditorApplication&>(Engine::Application::Get()); }

        virtual void OnInitialized() override;

        void ApplyScene(const std::string& sceneId);

        std::shared_ptr<Engine::SceneAsset> GetActiveSceneAsset();
        std::shared_ptr<Engine::Scene> GetActiveScene();
        std::shared_ptr<EditorCameraController> GetCameraController() const { return cameraController_; }
        
        void AddDebugRay(Engine::Ray ray) { debugRays_.push_back(ray);  }
        const std::vector<Engine::Ray>& GetDebugRays() const { return debugRays_; }

    private:
        void LoadStartupScene();

    private:
        std::shared_ptr<SceneLayer> sceneLayer_;
        std::shared_ptr<GizmoLayer> gizmoLayer_;
        std::shared_ptr<ImGuiLayer> imGuiLayer_;

        std::shared_ptr<EditorCameraController> cameraController_;
        std::string activeSceneId_;
        std::vector<Engine::Ray> debugRays_;
    };

}

