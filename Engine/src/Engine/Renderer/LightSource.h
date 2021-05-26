#pragma once
#include <DirectXMath.h>

namespace Engine
{

    class Transform;

    class LightSource
    {
    public:
        std::shared_ptr<Transform> GetTransform() const { return m_transform; }
        void SetTransform(std::shared_ptr<Transform> transform) { m_transform = transform; }

        DirectX::XMFLOAT3A GetColor() const { return m_color; }
        void SetColor(DirectX::XMFLOAT3A color) { m_color = color; }
        void SetColor(float r, float g, float b) { m_color = { r, g, b }; }

    private:
        std::shared_ptr<Transform> m_transform;
        DirectX::XMFLOAT3A m_color = { 1, 1, 1 };
    };
}

