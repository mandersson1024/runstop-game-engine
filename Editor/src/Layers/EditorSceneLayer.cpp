#include "EditorSceneLayer.h"
#include <vector>
#include "EditorApplication.h"
#include "EditorCameraController.h"
#include "Engine\Application\Application.h"
#include "Engine\Platform\Input.h"
#include "Engine\Scene\Scene.h"
#include "Engine\Application\MouseEvent.h"
#include "Engine\Renderer\IndexBuffer.h"
#include "Engine\Renderer\VertexTypes.h"
#include "Engine\Renderer\VertexBuffer.h"
#include "Engine\Renderer\Renderer.h"
#include "Engine\Renderer\Mesh.h"
#include "Engine\ProjectSettings.h"
#include "Engine\Scene\Camera.h"
#include "Engine\Scene\Transform.h"
#include "Engine\Assets\Assets.h"

namespace Editor
{
    struct GridMesh
    {
        std::vector<float> vertexData;
        std::vector<Engine::IndexBuffer::IndexType> indexData;

        static constexpr int c_numLines = 11;
        static constexpr float c_length = (c_numLines - 1) / 2;
        static constexpr float c_spacing = 1;

        static std::shared_ptr<Engine::Mesh> Create()
        {
            GridMesh grid;
            
            for (int i = 0; i < c_length + 1; ++i)
            {
                grid.PushLine(i, c_spacing);

                if (i != 0)
                {
                    grid.PushLine(i, -c_spacing);
                }
            }

            auto vertices = std::make_shared<Engine::VertexBuffer>(grid.vertexData, Engine::GetVertexTypeSize(Engine::VertexType::Minimal));
            auto indices = std::make_shared<Engine::IndexBuffer>(grid.indexData);
            return std::make_shared<Engine::Mesh>(vertices, indices, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        }

        void IncreaseIndex()
        {
            indexData.push_back(static_cast<Engine::IndexBuffer::IndexType>(indexData.size()));
        }

        void PushLine(int i, float distance)
        {
            PushLineX(i * distance);
            PushLineZ(i * distance);
        }

        void PushLineX(float x)
        {
            vertexData.push_back(x);
            vertexData.push_back(0);
            vertexData.push_back(-c_length);
            IncreaseIndex();

            vertexData.push_back(x);
            vertexData.push_back(0);
            vertexData.push_back(c_length);
            IncreaseIndex();
        }

        void PushLineZ(float z)
        {
            vertexData.push_back(-c_length);
            vertexData.push_back(0);
            vertexData.push_back(z);
            IncreaseIndex();

            vertexData.push_back(c_length);
            vertexData.push_back(0);
            vertexData.push_back(z);
            IncreaseIndex();
        }
    };

    struct FrustumMesh
    {
        static std::shared_ptr<Engine::Mesh> Create()
        {
            std::vector<float> vertexData =
            {
                // near plane
                -1, -1, 0,
                -1,  1, 0,
                 1,  1, 0,
                 1, -1, 0,

                // far plane
                -1, -1, 1,
                -1,  1, 1,
                 1,  1, 1,
                 1, -1, 1,

                // far cross
                 0, -1, 1,
                 0,  1, 1,
                -1,  0, 1,
                 1,  0, 1,
            };

            std::vector<Engine::IndexBuffer::IndexType> indexData =
            {
                // near square
                0, 1,
                1, 2,
                2, 3,
                3, 0,

                // far square
                4, 5,
                5, 6,
                6, 7, 
                7, 4,

                // connections
                0, 4,
                1, 5,
                2, 6,
                3, 7,

                // far cross
                8, 9,
                10, 11,
            };

            auto vertices = std::make_shared<Engine::VertexBuffer>(vertexData, Engine::GetVertexTypeSize(Engine::VertexType::Minimal));
            auto indices = std::make_shared<Engine::IndexBuffer>(indexData);
            
            return std::make_shared<Engine::Mesh>(vertices, indices, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        }
    };

    struct HeightPillarMesh
    {
        static std::shared_ptr<Engine::Mesh> Create(DirectX::XMVECTOR position)
        {
            const float topBottomLength = 0.05f;
            
            std::vector<float> vertexData =
            {
                // bottom cross
                DirectX::XMVectorGetX(position) - topBottomLength,
                0,
                DirectX::XMVectorGetZ(position),
                DirectX::XMVectorGetX(position) + topBottomLength,
                0,
                DirectX::XMVectorGetZ(position),

                DirectX::XMVectorGetX(position),
                0,
                DirectX::XMVectorGetZ(position) - topBottomLength,
                DirectX::XMVectorGetX(position),
                0,
                DirectX::XMVectorGetZ(position) + topBottomLength,

                // vertical line
                DirectX::XMVectorGetX(position),
                0,
                DirectX::XMVectorGetZ(position),
                DirectX::XMVectorGetX(position),
                DirectX::XMVectorGetY(position),
                DirectX::XMVectorGetZ(position),

                // top cross
                DirectX::XMVectorGetX(position) - topBottomLength,
                DirectX::XMVectorGetY(position),
                DirectX::XMVectorGetZ(position),
                DirectX::XMVectorGetX(position) + topBottomLength,
                DirectX::XMVectorGetY(position),
                DirectX::XMVectorGetZ(position),

                DirectX::XMVectorGetX(position),
                DirectX::XMVectorGetY(position),
                DirectX::XMVectorGetZ(position) - topBottomLength,
                DirectX::XMVectorGetX(position),
                DirectX::XMVectorGetY(position),
                DirectX::XMVectorGetZ(position) + topBottomLength,
            };

            std::vector<Engine::IndexBuffer::IndexType> indexData =
            {
                0, 1, 2, 3,
                4, 5,
                6, 7, 8, 9,
            };

            auto vertices = std::make_shared<Engine::VertexBuffer>(vertexData, Engine::GetVertexTypeSize(Engine::VertexType::Minimal));
            auto indices = std::make_shared<Engine::IndexBuffer>(indexData);

            return std::make_shared<Engine::Mesh>(vertices, indices, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        }
    };

    struct RayMesh
    {
        std::vector<float> vertexData;
        std::vector<Engine::IndexBuffer::IndexType> indexData;

        static std::shared_ptr<Engine::Mesh> Create(const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2)
        {
            std::vector<float> vertexData =
            {
                DirectX::XMVectorGetX(p1),
                DirectX::XMVectorGetY(p1),
                DirectX::XMVectorGetZ(p1),

                DirectX::XMVectorGetX(p2),
                DirectX::XMVectorGetY(p2),
                DirectX::XMVectorGetZ(p2),
            };

            std::vector<Engine::IndexBuffer::IndexType> indexData = { 0, 1 };

            auto vertices = std::make_shared<Engine::VertexBuffer>(vertexData, Engine::GetVertexTypeSize(Engine::VertexType::Minimal));
            auto indices = std::make_shared<Engine::IndexBuffer>(indexData);

            return std::make_shared<Engine::Mesh>(vertices, indices, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        }
    };

    SceneLayer::SceneLayer()
        : Layer("Scene Layer")
        , grid_(GridMesh::Create())
        , frustum_(FrustumMesh::Create())
    {
    }

    void SceneLayer::OnUpdate(float deltaTime)
    {
        auto scene = EditorApplication::Get().GetActiveScene();
        
        Engine::Application::GetRenderer().BeginScene();

        if (Engine::Application::GetProjectSettings().GetDrawHelperLines())
        {
            Engine::Application::GetRenderer().SubmitMesh(grid_, "materials/editor/grid", DirectX::XMMatrixIdentity(), false, false);

            std::vector<std::shared_ptr<Engine::Camera>> cameras;
            scene->AddAllCamerasToVector(cameras);
            for (const auto& camera : cameras)
            {
                auto frustumMatrix = DirectX::XMMatrixInverse(nullptr, camera->GetViewProjectionMatrix());
                auto frustumMaterial = camera->GetTransform()->HasLightSource() ? "materials/editor/light_source_frustum" : "materials/editor/camera_frustum";
                Engine::Application::GetRenderer().SubmitMesh(frustum_, frustumMaterial, frustumMatrix, false, false);

                const auto pillar = HeightPillarMesh::Create(camera->GetTransform()->GetPosition());
                Engine::Application::GetRenderer().SubmitMesh(pillar, frustumMaterial, DirectX::XMMatrixIdentity(), false, false);
            }
        }

        {
            for (const auto& ray : EditorApplication::Get().GetDebugRays())
            {
                const auto endPoint = DirectX::XMVectorAdd(ray.origin, DirectX::XMVectorScale(ray.direction, 20));
                auto mesh = RayMesh::Create(ray.origin, endPoint);
                Engine::Application::GetRenderer().SubmitMesh(mesh, "materials/editor/ray", DirectX::XMMatrixIdentity(), false, false);
            }
        }

        if (Engine::Application::GetProjectSettings().GetRunInEditor())
        {
            scene->OnUpdateControllers(deltaTime);
        }

        scene->OnUpdate(deltaTime);

        const auto camera = EditorApplication::GetRenderer().GetCamera();

        Engine::Application::GetRenderer().ClearEnvironment();

        if (scene->HasEnvironmentMap())
        {
            const auto skyboxMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslationFromVector(camera->GetTransform()->GetPosition()));
            const auto skyboxMesh = Engine::Assets::GetMesh("skybox");
            const auto skyboxMaterial = Engine::Application::GetProjectSettings().GetDrawSkyboxUsingPointSampler() ? "skybox_point" : "skybox";
            const auto environmentId = scene->GetEnvironmentMapId();

            // TODO: The switch cases are all the same

            switch (static_cast<SkyboxDisplayType>(Engine::Application::GetProjectSettings().GetSkyboxDisplayType()))
            {
            case SkyboxDisplayType::Skybox:
                Engine::Application::GetRenderer().SetEnvironment(
                    environmentId,
                    skyboxMesh,
                    skyboxMaterial,
                    skyboxMatrix,
                    0);
                break;
            case SkyboxDisplayType::Irradiance:
                Engine::Application::GetRenderer().SetEnvironment(
                    environmentId,
                    skyboxMesh,
                    skyboxMaterial,
                    skyboxMatrix,
                    0);
                break;
            case SkyboxDisplayType::Radiance:
                Engine::Application::GetRenderer().SetEnvironment(
                    environmentId,
                    skyboxMesh,
                    skyboxMaterial,
                    skyboxMatrix,
                    Engine::Application::GetProjectSettings().GetRadianceMapRoughness());
                break;
            }
        }

        if (!Engine::Application::GetProjectSettings().GetRunInEditor())
        {
            EditorApplication::Get().GetCameraController()->OnUpdate(deltaTime);
        }

        Engine::Application::GetRenderer().SetDebug(
            Engine::Application::GetProjectSettings().GetIgnoreLightSources(),
            Engine::Application::GetProjectSettings().GetIgnoreDiffuseIBL(),
            Engine::Application::GetProjectSettings().GetIgnoreSpecularIBL(),
            Engine::Application::GetProjectSettings().GetIgnoreAmbientOcclusion(),
            static_cast<SkyboxDisplayType>(Engine::Application::GetProjectSettings().GetSkyboxDisplayType()) == SkyboxDisplayType::Radiance,
            static_cast<SkyboxDisplayType>(Engine::Application::GetProjectSettings().GetSkyboxDisplayType()) == SkyboxDisplayType::Irradiance
        );

        Engine::Application::GetRenderer().RenderScene();
    }

    void SceneLayer::OnEvent(Engine::Event& evt)
    {
        auto scene = EditorApplication::Get().GetActiveScene();

        if (Engine::Application::GetProjectSettings().GetRunInEditor())
        {
            scene->OnEvent(evt);
        }
        else
        {
            if (evt.GetEventType() == Engine::EventType::MouseScroll)
            {
                float delta = static_cast<Engine::MouseScrollEvent&>(evt).GetDelta();
                EditorApplication::Get().GetCameraController()->OnMouseScroll(delta);
                evt.handled = true;
            }

            if (evt.GetEventType() == Engine::EventType::MouseDown)
            {
                if (static_cast<Engine::MouseDownEvent&>(evt).GetButton() == 0 && Engine::Input::Get()->IsKeyDown(DirectX::Keyboard::R))
                {
                    // Cast a ray at mouse screen position from camera's near to far clipping plane

                    const auto& mouse = Engine::Input::Get()->GetMouse().GetState();
                    const auto camera = EditorApplication::GetRenderer().GetCamera();
                    const Engine::Ray ray = camera->ScreenPointToRay(mouse.x, mouse.y);
                    EditorApplication::Get().AddDebugRay(ray);
                    evt.handled = true;
                }
            }
        }
    }

    void SceneLayer::OnAttach()
    {
    }

}

