#include "GizmoLayer.h"
#include <DirectXMath.h>
#include "Engine\Renderer\Mesh.h"
#include "Engine\Renderer\Renderer.h"
#include "Engine\Renderer\VertexBuffer.h"
#include "Engine\Renderer\IndexBuffer.h"
#include "Engine\Renderer\VertexTypes.h"
#include "Engine\Scene\Camera.h"
#include "Engine\Scene\Scene.h"
#include "EditorApplication.h"
#include "Engine\Application\Application.h"
#include "Engine\Assets\Model.h"
#include "Engine\Platform\Input.h"
#include "Engine\ProjectSettings.h"
#include "Engine\Scene\Transform.h"
#include "Engine\Assets\Assets.h"

namespace Editor
{

    struct BoxColliderMesh
    {
        static std::shared_ptr<Engine::Mesh> Create(const DirectX::XMVECTOR verts[8])
        {
            std::vector<float> vertexData =
            {
                DirectX::XMVectorGetX(verts[0]),
                DirectX::XMVectorGetY(verts[0]),
                DirectX::XMVectorGetZ(verts[0]),

                DirectX::XMVectorGetX(verts[1]),
                DirectX::XMVectorGetY(verts[1]),
                DirectX::XMVectorGetZ(verts[1]),

                DirectX::XMVectorGetX(verts[2]),
                DirectX::XMVectorGetY(verts[2]),
                DirectX::XMVectorGetZ(verts[2]),

                DirectX::XMVectorGetX(verts[3]),
                DirectX::XMVectorGetY(verts[3]),
                DirectX::XMVectorGetZ(verts[3]),

                DirectX::XMVectorGetX(verts[4]),
                DirectX::XMVectorGetY(verts[4]),
                DirectX::XMVectorGetZ(verts[4]),

                DirectX::XMVectorGetX(verts[5]),
                DirectX::XMVectorGetY(verts[5]),
                DirectX::XMVectorGetZ(verts[5]),

                DirectX::XMVectorGetX(verts[6]),
                DirectX::XMVectorGetY(verts[6]),
                DirectX::XMVectorGetZ(verts[6]),

                DirectX::XMVectorGetX(verts[7]),
                DirectX::XMVectorGetY(verts[7]),
                DirectX::XMVectorGetZ(verts[7]),
            };

            std::vector<Engine::IndexBuffer::IndexType> indexData = 
            { 
                // front
                0, 1, 
                1, 2, 
                2, 3, 
                3, 0,

                // back
                4, 5, 
                5, 6, 
                6, 7, 
                7, 4, 

                // top
                0, 4,
                1, 5,

                // bottom
                2, 6,
                3, 7,
            };

            auto vertices = std::make_shared<Engine::VertexBuffer>(vertexData, Engine::GetVertexTypeSize(Engine::VertexType::Minimal));
            auto indices = std::make_shared<Engine::IndexBuffer>(indexData);

            return std::make_shared<Engine::Mesh>(vertices, indices, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        }
    };


    GizmoLayer::GizmoLayer() : Layer("Gizmo Layer")
    {
    }

    void GizmoLayer::OnUpdate(float deltaTime)
    {
        auto scene = EditorApplication::Get().GetActiveScene();

        Engine::Application::GetRenderer().BeginGizmos();

        if (Engine::Application::GetProjectSettings().GetDrawColliders())
        {
            for (const auto transform : scene->GetTransforms())
            {
                FindAndRenderColliders(transform);
            }
        }
    }

    void GizmoLayer::FindAndRenderColliders(std::shared_ptr<Engine::Transform> transform)
    {
        auto quadModel = Engine::Assets::GetModel("models/quad/quad.obj");

        if (quadModel->parts.empty())
        {
            //
            // This usually means that the quad model has not finished loading
            //

            return;
        }

        auto quad = quadModel->parts[0].mesh;

        auto targetMaterial = Engine::Assets::GetMaterial("materials/editor/raycast_target");
        auto hitMaterial = Engine::Assets::GetMaterial("materials/editor/raycast_hit");

        if (transform->HasCollider())
        {
            const auto target = transform->GetCollider();
            DirectX::XMVECTOR verts[8];
            target->GetVertices(verts);
            Engine::Application::GetRenderer().RenderGizmo(BoxColliderMesh::Create(verts), targetMaterial, DirectX::XMMatrixIdentity());

            for (const auto& ray : EditorApplication::Get().GetDebugRays())
            {
                const auto hit = target->Raycast(ray);

                if (hit.valid)
                {
                    Engine::Application::GetRenderer().RenderGizmo(quad, hitMaterial, GetRaycastHitGizmoMatrix(hit));
                }
            }

            if (Engine::Input::Get()->IsKeyDown(DirectX::Keyboard::R))
            {
                // Draw a raycast hit gizmo at mouse position if 'R' key is down

                const auto& mouse = Engine::Input::Get()->GetMouse().GetState();
                const auto camera = Engine::Application::GetRenderer().GetCamera();
                const auto ray = camera->ScreenPointToRay(mouse.x, mouse.y);
                const auto hit = target->Raycast(ray);

                if (hit.valid)
                {
                    Engine::Application::GetRenderer().RenderGizmo(quad, hitMaterial, GetRaycastHitGizmoMatrix(hit));
                }
            }

        }

        for (auto child : transform->GetChildren())
        {
            FindAndRenderColliders(child);
        }
    }

    DirectX::XMMATRIX GizmoLayer::GetRaycastHitGizmoMatrix(Engine::RaycastHit hit) const
    {
        const auto v0 = DirectX::XMVector4Normalize(hit.normal);
        const auto v1 = DirectX::XMVector4Normalize(DirectX::XMVector3Orthogonal(v0));
        const auto v2 = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(v0, v1));
        const auto orientation = DirectX::XMMatrixSet(
            DirectX::XMVectorGetX(v2),
            DirectX::XMVectorGetY(v2),
            DirectX::XMVectorGetZ(v2),
            0,
            DirectX::XMVectorGetX(v1),
            DirectX::XMVectorGetY(v1),
            DirectX::XMVectorGetZ(v1),
            0,
            DirectX::XMVectorGetX(v0),
            DirectX::XMVectorGetY(v0),
            DirectX::XMVectorGetZ(v0),
            0,
            0, 0, 0, 1);

        return DirectX::XMMatrixTranspose(
            DirectX::XMMatrixScaling(0.2f, 0.2f, 1) *
            orientation *
            DirectX::XMMatrixTranslationFromVector(hit.point)
        );
    }

}

