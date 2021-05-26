#include "EditorApplication.h"
#include "EditorCameraController.h"
#include "Layers/EditorSceneLayer.h"
#include "Layers/ImGuiLayer.h"
#include "Layers/GizmoLayer.h"
#include "Engine/Main.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Application/Application.h"
#include "Engine/ProjectSettings.h"
#include "Engine/Scene/Camera.h"
#include "Engine/Scene/Transform.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Assets/Assets.h"


std::shared_ptr<Engine::Application> Engine::CreateApplication()
{
    return  std::make_shared<Editor::EditorApplication>();
}

namespace Editor
{

    EditorApplication::EditorApplication()
        : Application()
        , cameraController_(std::make_shared<EditorCameraController>())
    {
        ENGINE_LOG_INFO("Creating EditorApplication");
    }

    void EditorApplication::OnInitialized()
    {
        Engine::Application::GetRenderer().SetCamera(cameraController_->GetCamera());

        sceneLayer_ = std::make_shared<SceneLayer>();
        PushLayer(sceneLayer_);

        gizmoLayer_ = std::make_shared <GizmoLayer>();
        PushLayer(gizmoLayer_);

        // We want the ImGuiLayer to be on the top of the stack.
        // For now we assume that no other layers will be pushed
        // after the application has started.

        imGuiLayer_ = std::make_shared <ImGuiLayer>(GetWindow());
        PushLayer(imGuiLayer_);

        LoadStartupScene();
    }

    void EditorApplication::LoadStartupScene()
    {
        auto& projectSettings = Engine::Application::GetProjectSettings();
        const auto& startupScene = projectSettings.GetStartupScene();
        const auto& allScenes = Engine::Assets::GetAllSceneAssets();

        if (allScenes.contains(startupScene))
        {
            ApplyScene(startupScene);
        }
        else
        {
            ENGINE_LOG_WARNING("Didn't find '{}'", startupScene);
            ENGINE_ASSERT(allScenes.size() > 0, "Didn't find any scene");

            auto firstScene = allScenes.begin()->first;
            ENGINE_LOG_WARNING("Reverting to '{}'", firstScene);
            projectSettings.SetStartupScene(firstScene);
            ApplyScene(firstScene);
        }
    }

    void EditorApplication::ApplyScene(const std::string& sceneId)
    {
        activeSceneId_ = sceneId;
        auto scene = GetActiveScene();
        auto mainCamera = scene->GetCamera("main_camera");
        ENGINE_ASSERT(mainCamera != nullptr, "Every scene needs a main camera");

        {
            // This is a way to set the transform, but keep the freefly defaults
            // for FOV and clipping planes.
            
            EditorApplication::Get().GetCameraController()->GetCamera()->SetFromOtherCamera(mainCamera);
            EditorApplication::Get().GetCameraController()->Reset();
        }
    }

    std::shared_ptr<Engine::SceneAsset> EditorApplication::GetActiveSceneAsset()
    {
        return Engine::Assets::GetSceneAsset(activeSceneId_);
    }

    std::shared_ptr<Engine::Scene> EditorApplication::GetActiveScene()
    {
        return Engine::Assets::GetScene(activeSceneId_);
    }

}

