#pragma once
#include <memory>
#include <DirectXMath.h>

namespace Engine
{

    class Transform;
    struct Ray;

    enum class CameraType
    {
        Orthographic,
        Perspective,
    };

    class Camera
    {
    public:
        Camera();

        void SetFromOtherCamera(const std::shared_ptr<Camera> other);

        std::shared_ptr<Transform> GetTransform() const { return m_transform; }
        DirectX::XMVECTOR GetPosition() const;
        //DirectX::XMVECTOR GetWorldPosition() const { return m_transform->GetWorldPosition(); }
        float GetFieldOfView() const { return m_fov; }
        float GetNearClippingPlane() const { return m_nearClippingPlane; }
        float GetFarClippingPlane() const { return m_farClippingPlane; }
        float GetOrthoViewWidth() const { return m_orthoViewWidth; }
        float GetOrthoViewHeight() const { return m_orthoViewHeight; }
        CameraType GetType() const { return m_type; }

        void SetPosition(DirectX::XMVECTOR position);
        void SetPosition(float x, float y, float z);
        void SetFieldOfView(float fov);
        void SetNearClippingPlane(float nearClip);
        void SetFarClippingPlane(float farClip);
        void SetOrthoViewWidth(float width) { m_orthoViewWidth = width; }
        void SetOrthoViewHeight(float height) { m_orthoViewHeight = height; }
        void SetType(CameraType type);
        void SetTransform(std::shared_ptr<Transform> transform) { m_transform = transform; }

        void Move(DirectX::XMVECTOR direction, float distance);
        void Rotate(float xAxis, float yAxis);

        DirectX::XMMATRIX GetViewMatrix() const;
        DirectX::XMMATRIX GetProjectionMatrix() const;
        DirectX::XMMATRIX GetViewProjectionMatrix() const;

        Ray ScreenPointToRay(int x, int y) const;
        DirectX::XMVECTOR WorldPositionToScreenPoint(DirectX::XMVECTOR position) const;
        DirectX::XMVECTOR WorldDirectionToScreenDirection(DirectX::XMVECTOR direction) const;

    private:
        DirectX::XMMATRIX GetOrthographicProjectionMatrix() const;
        DirectX::XMMATRIX GetPerspectiveProjectionMatrix() const;

    private:
        CameraType m_type = CameraType::Perspective;
        float m_nearClippingPlane = 0.1f;
        float m_farClippingPlane = 20;
        float m_fov;
        float m_orthoViewWidth;
        float m_orthoViewHeight;
        std::shared_ptr<Transform> m_transform;
    };

}

