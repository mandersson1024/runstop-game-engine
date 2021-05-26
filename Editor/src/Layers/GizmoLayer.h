#pragma once
#include <memory>
#include <DirectXMath.h>
#include "Engine\Application\Layer.h"

namespace Engine
{
    class Transform;
    struct RaycastHit;
}

namespace Editor
{
    class GizmoLayer : public Engine::Layer
    {
    public:
        GizmoLayer();

        void OnUpdate(float deltaTime) override;

    private:
        void FindAndRenderColliders(std::shared_ptr<Engine::Transform> transform);
        DirectX::XMMATRIX GetRaycastHitGizmoMatrix(Engine::RaycastHit hit) const;
    };

}
