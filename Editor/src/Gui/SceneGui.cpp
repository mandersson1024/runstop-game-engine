#include "SceneGui.h"
#include "ImGui/imgui.h"
#include "EditorApplication.h"
#include "Engine/Assets/SceneAsset.h"
#include "Engine/ProjectSettings.h"
#include "Engine/Scene/Scene.h"
#include "EditorCameraController.h"
#include "spdlog/fmt/fmt.h"
#include "Engine/Scene/GameObject.h"
#include "Engine/Assets/Model.h"
#include "Engine/Utils/MathUtils.h"
#include "Engine/Renderer/LightSource.h"
#include "Engine/Scene/Camera.h"
#include "Engine/Scene/Transform.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Assets/Assets.h"

namespace Editor
{

    void SceneGui::Draw()
    {
        ImGui::Begin("Scene");

        auto scene = EditorApplication::Get().GetActiveScene();
        
        {
            //
            // RUN/STOP button
            //

            bool wasRunning = Engine::Application::GetProjectSettings().GetRunInEditor();

            if (wasRunning)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0.6f, 0.2f, 1 });
            }

            if (ImGui::Button(wasRunning ? "Stop" : "Run"))
            {
                Engine::Application::GetProjectSettings().SetRunInEditor(!wasRunning);
            }

            if (wasRunning)
            {
                ImGui::PopStyleColor();
            }
        }

        {
            //
            // SAVE button
            //

            ImGui::SameLine(ImGui::GetWindowWidth()- 102);

            if (ImGui::Button("Save"))
            {
                EditorApplication::Get().GetActiveSceneAsset()->Save();
            }
        }

        {
            //
            // REVERT button
            //

            ImGui::SameLine();

            if (ImGui::Button("Revert"))
            {
                EditorApplication::Get().GetActiveSceneAsset()->SetLoadingState(Engine::AssetLoadingState::Loading);
                EditorApplication::Get().GetActiveSceneAsset()->LoadSync();
                EditorApplication::Get().GetActiveSceneAsset()->SetLoadingState(Engine::AssetLoadingState::Loaded);
            }
        }

        ImGui::Separator();

        {
            //
            // ENVIRONMENT MAP
            //

            //ImGui::SetNextItemOpen(true);
            if (ImGui::TreeNode("Environment Map"))
            {
                const auto selected = scene->GetEnvironmentMapId();

                for (const auto& map : Engine::Assets::GetAllEnvironmentMapAssets())
                {
                    const std::string& id = map.first.c_str();

                    if (ImGui::Selectable(id.c_str(), id == selected))
                    {
                        scene->SetEnvironmentMapId(id);
                    }
                }

                ImGui::Separator();
                ImGui::TreePop();
            }
        }

        auto light = scene->GetLightSource();
        DrawDirectionalLight(light->GetTransform());

        auto& transforms = scene->GetTransforms();
        for (int i = 0; i < transforms.size(); ++i)
        {
            auto transform = transforms[i];

            if (transform->HasGameObject())
            {
                DrawGameObject(transform, i);
            }

            if (transform->HasCamera())
            {
                DrawCamera(transform, i);
            }
        }

        ImGui::End();
    }

    void SceneGui::DrawCollider(std::shared_ptr<Engine::Transform> transform)
    {
        //ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode("Box Collider"))
        {
            auto collider = transform->GetCollider();

            {
                float size[3]
                {
                    collider->GetSizeX(),
                    collider->GetSizeY(),
                    collider->GetSizeZ(),
                };

                if (ImGui::DragFloat3("Size", size, 0.01f))
                {
                    collider->SetSize(size[0], size[1], size[2]);
                }
            }

            {
                float offset[3]
                {
                    collider->GetOffsetX(),
                    collider->GetOffsetY(),
                    collider->GetOffsetZ(),
                };

                if (ImGui::DragFloat3("Offset", offset, 0.01f))
                {
                    collider->SetOffset(offset[0], offset[1], offset[2]);
                }
            }

            ImGui::TreePop();
        }
    }

    void SceneGui::DrawCamera(std::shared_ptr<Engine::Transform> transform, int imGuiWidgetId)
    {
        auto camera = transform->GetCamera();
        auto freeFlyCamera = EditorApplication::Get().GetCameraController()->GetCamera();
        ImGui::PushID(imGuiWidgetId);

        //ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode(std::format("Camera ({})", transform->GetName()).c_str()))
        {
            int cameraType = static_cast<int>(camera->GetType());
            if (ImGui::RadioButton("Orthographic", &cameraType, static_cast<int>(Engine::CameraType::Orthographic)))
            {
                camera->SetType(Engine::CameraType::Orthographic);

                if (m_selectedCamera == camera)
                {
                    freeFlyCamera->SetType(Engine::CameraType::Orthographic);
                }
            }

            if (bool pressed2 = ImGui::RadioButton("Perspective", &cameraType, static_cast<int>(Engine::CameraType::Perspective)))
            {
                camera->SetType(Engine::CameraType::Perspective);

                if (m_selectedCamera == camera)
                {
                    freeFlyCamera->SetType(Engine::CameraType::Perspective);
                }
            }

            float fov = camera->GetFieldOfView();
            if (ImGui::SliderAngle("Field of view", &fov, 10.f, 160.f))
            {
                camera->SetFieldOfView(fov);

                if (m_selectedCamera == camera)
                {
                    freeFlyCamera->SetFieldOfView(fov);
                }
            }

            {
                float nearClip = camera->GetNearClippingPlane();
                if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.1f, camera->GetFarClippingPlane() - 0.1f))
                {
                    camera->SetNearClippingPlane(nearClip);

                    if (m_selectedCamera == camera)
                    {
                        freeFlyCamera->SetNearClippingPlane(nearClip);
                    }
                }
            }

            {
                float orthoViewWidthHeight[2] =
                {
                    camera->GetOrthoViewWidth(),
                    camera->GetOrthoViewHeight(),
                };

                if (ImGui::DragFloat2("Ortho Size", orthoViewWidthHeight, 0.1f, 1, 50))
                {
                    camera->SetOrthoViewWidth(orthoViewWidthHeight[0]);
                    camera->SetOrthoViewHeight(orthoViewWidthHeight[1]);
                }
            }

            {
                float farClip = camera->GetFarClippingPlane();
                if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, camera->GetNearClippingPlane() + 0.1f, 50.f))
                {
                    camera->SetFarClippingPlane(farClip);

                    if (m_selectedCamera == camera)
                    {
                        freeFlyCamera->SetFarClippingPlane(farClip);
                    }
                }
            }

            if (DrawTransform(transform, 0))
            {
                if (m_selectedCamera == camera)
                {
                    EditorApplication::Get().GetCameraController()->GetCamera()->SetFromOtherCamera(camera);
                }
            }

            bool selected = m_selectedCamera == camera;
            if (ImGui::Checkbox("Select", &selected))
            {
                if (selected && m_selectedCamera != camera)
                {
                    EditorApplication::Get().GetCameraController()->GetCamera()->SetFromOtherCamera(camera);
                    m_selectedCamera = camera;
                }
                else
                {
                    EditorApplication::Get().GetCameraController()->Reset();
                    m_selectedCamera.reset();
                }
            }
            
            if (m_selectedCamera == camera)
            {
                camera->SetFromOtherCamera(EditorApplication::Get().GetCameraController()->GetCamera());
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void SceneGui::DrawDirectionalLight(std::shared_ptr<Engine::Transform> transform)
    {
        auto light = transform->GetLightSource();

        //ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode("Light Source"))
        {
            const auto& color = light->GetColor();
            float col[3] { color.x, color.y, color.z };

            if (ImGui::DragFloat3("Color", &col[0], 0.1f, 0, 50))
            {
                light->SetColor(col[0], col[1], col[2]);
            }

            DrawCamera(transform, 0);
            DrawTransform(transform, 0);

            ImGui::TreePop();
        }
    }

    void SceneGui::DrawGameObject(std::shared_ptr<Engine::Transform> transform, int imGuiWidgetId)
    {
        ImGui::PushID(imGuiWidgetId);

        auto gameObject = transform->GetGameObject();

        //ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode(std::format("GameObject ({})", transform->GetName()).c_str()))
        {
            bool active = gameObject->IsActive();
            if (ImGui::Checkbox("Active", &active))
            {
                gameObject->SetActive(active);
            }

            DrawTransform(transform, 0);

            if (transform->HasCollider())
            {
                DrawCollider(transform);
            }

            ImGui::Text("Materials");
            ImGui::Indent();

            std::shared_ptr<Engine::Model> model;
            if (gameObject->HasModel())
            {
                model = Engine::Assets::GetModel(gameObject->GetModelId());
            }

            for (int i = 0; i < gameObject->GetMaterialIds().size(); ++i)
            {
                bool isMaterialUnused = !gameObject->HasModel() || i >= model->parts.size();
                if (isMaterialUnused)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, { 0.8f, 0.2f, 0.8f, 1 });
                }

                auto material = gameObject->GetMaterialIds()[i];
                ImGui::Text(std::format("Index {} ({})", i, material).c_str());

                if (isMaterialUnused)
                {
                    ImGui::PopStyleColor();
                }
            }
            ImGui::Unindent();

            if (gameObject->HasModel())
            {
                ImGui::Text("Model");
                ImGui::Indent();

                auto& parts = model->parts;

                for (int i = 0; i < parts.size(); ++i)
                {
                    DrawModelPart(gameObject, parts[i], i);
                }

                ImGui::Unindent();
            }

            if (transform->HasChildren())
            {
                ImGui::Text("Children");
                ImGui::Indent();

                auto children = transform->GetChildren();

                for (int i = 0; i < children.size(); ++i)
                {
                    DrawGameObject(children[i], i);
                }

                ImGui::Unindent();
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    bool SceneGui::DrawTransform(std::shared_ptr<Engine::Transform> transform, int imGuiWidgetId)
    {
        bool dirty = false;

        if (ImGui::TreeNode("Transform"))
        {
            dirty = dirty || DrawPosition(transform);
            dirty = dirty || DrawScale(transform);
            dirty = dirty || DrawOrientation(transform);

            ImGui::TreePop();
        }

        return dirty;
    }

    bool SceneGui::DrawPosition(std::shared_ptr<Engine::Transform> transform)
    {
        bool dirty = false;

        auto& position = transform->GetPosition();

        float pos[3]
        {
            DirectX::XMVectorGetX(position),
            DirectX::XMVectorGetY(position),
            DirectX::XMVectorGetZ(position),
        };

        if (ImGui::DragFloat3("Position", pos, 0.01f))
        {
            transform->SetPosition(pos[0], pos[1], pos[2]);
            dirty = true;
        }

        return dirty;
    }

    bool SceneGui::DrawOrientation(std::shared_ptr<Engine::Transform> transform)
    {
        bool dirty = false;
        auto& orientation = transform->GetOrientation();

        {
            Engine::EulerAngles angles = transform->GetEulerAngles();

            float a[3]
            {
                Engine::RadiansToDegrees(angles.x),
                Engine::RadiansToDegrees(angles.y),
                Engine::RadiansToDegrees(angles.z)
            };

            if (ImGui::DragFloat3("Orientation", a, 0.2f, -180, 180))
            {
                Engine::EulerAngles newAngles
                {
                    .x = Engine::DegreesToRadians(a[0]),
                    .y = Engine::DegreesToRadians(a[1]),
                    .z = Engine::DegreesToRadians(a[2])
                };

                transform->SetEulerAngles(newAngles);
                dirty = true;
            }
        }

        {
            ImGui::Text(std::format("Quaternion: w: {:.1}, x: {:.1}, y: {:.1}, z: {:.1}",
                DirectX::XMVectorGetW(orientation),
                DirectX::XMVectorGetX(orientation),
                DirectX::XMVectorGetY(orientation),
                DirectX::XMVectorGetZ(orientation)).c_str());
        }

        {
            ImGui::PushButtonRepeat(true);

            if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
            { 
                transform->RotateAroundLocalAxisX(Engine::DegreesToRadians(2)); dirty = true; 
            }
            
            ImGui::SameLine(); ImGui::Text("X"); ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
            {
                transform->RotateAroundLocalAxisX(Engine::DegreesToRadians(-2)); dirty = true;
            }
            
            ImGui::SameLine(0, 25);
            if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
            { 
                transform->RotateAroundLocalAxisY(Engine::DegreesToRadians(2)); 
                dirty = true;
            }
            
            ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
            { 
                transform->RotateAroundLocalAxisY(Engine::DegreesToRadians(-2)); 
                dirty = true;
            }
            
            ImGui::SameLine(0, 25);
            if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
            { 
                transform->RotateAroundLocalAxisZ(Engine::DegreesToRadians(2)); 
                dirty = true;
            }
            
            ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
            { 
                transform->RotateAroundLocalAxisZ(Engine::DegreesToRadians(-2)); 
                dirty = true;
            }

            ImGui::SameLine(0, 25);
            ImGui::Text("Local Axis Rotation");

            if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
            { 
                transform->RotateAroundWorldAxisX(Engine::DegreesToRadians(2)); 
                dirty = true;
            }
            
            ImGui::SameLine(); ImGui::Text("X"); ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
            { 
                transform->RotateAroundWorldAxisX(Engine::DegreesToRadians(-2)); 
                dirty = true;
            }

            ImGui::SameLine(0, 25);
            if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
            { 
                transform->RotateAroundWorldAxisY(Engine::DegreesToRadians(2)); 
                dirty = true;
            }
            
            ImGui::SameLine(); ImGui::Text("Y"); ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
            { 
                transform->RotateAroundWorldAxisY(Engine::DegreesToRadians(-2)); 
                dirty = true;
            }
            
            ImGui::SameLine(0, 25);
            if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
            { 
                transform->RotateAroundWorldAxisZ(Engine::DegreesToRadians(2)); 
                dirty = true;
            }
            
            ImGui::SameLine(); ImGui::Text("Z"); ImGui::SameLine();
            if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
            { 
                transform->RotateAroundWorldAxisZ(Engine::DegreesToRadians(-2)); 
                dirty = true;
            }

            ImGui::SameLine(0, 25);
            ImGui::Text("World Axis Rotation");

            ImGui::PopButtonRepeat();
        }

        return dirty;
    }

    bool SceneGui::DrawScale(std::shared_ptr<Engine::Transform> transform)
    {
        bool dirty = false;
        auto& scale = transform->GetScale();

        float sca[3]
        {
            DirectX::XMVectorGetX(scale),
            DirectX::XMVectorGetY(scale),
            DirectX::XMVectorGetZ(scale),
        };

        if (ImGui::DragFloat3("Scale", sca, 0.01f))
        {
            transform->SetScale(sca[0], sca[1], sca[2]);
            dirty = true;
        }

        return dirty;
    }

    void SceneGui::DrawModelPart(std::shared_ptr<Engine::GameObject> gameObject, Engine::ModelPart& part, int imGuiWidgetId)
    {
        ImGui::PushID(imGuiWidgetId);

        if (ImGui::TreeNode(std::format("Part ({})", part.name).c_str()))
        {
            bool active = part.active;
            if (ImGui::Checkbox("Active", &active)) 
            {
                part.active = active;
            }
            DrawMesh(part.mesh);

            bool isMaterialMissing = part.materialIndex >= gameObject->GetMaterialIds().size();
            
            if (isMaterialMissing)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.8f, 0.2f, 0.8f, 1 });
            }
            
            ImGui::Text(std::format("Material: Index {} ({})", part.materialIndex, part.materialImportName).c_str());
            
            if (isMaterialMissing)
            {
                ImGui::PopStyleColor();
            }
            
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void SceneGui::DrawMesh(std::shared_ptr<Engine::Mesh> mesh)
    {
        auto& info = mesh->GetInfo();
        ImGui::Text("Mesh");
        ImGui::Indent();
        ImGui::Text(std::format("Name: {}", info.name).c_str());
        ImGui::Text(std::format("Triangle count: {}", info.triangleCount).c_str());
        ImGui::Unindent();
    }

}

