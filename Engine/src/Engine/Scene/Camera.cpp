#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "../EngineAssert.h"
#include "../Raycast/Raycast.h"
#include "../Utils/MathUtils.h"

namespace Engine
{

    Camera::Camera()
        : m_transform(std::make_shared<Transform>())
        , m_fov(DegreesToRadians(60))
        , m_orthoViewWidth(16)
        , m_orthoViewHeight(9)
    {
    }

    void Camera::SetFromOtherCamera(const std::shared_ptr<Engine::Camera> other)
    {
        m_type = other->m_type;
        m_nearClippingPlane = other->m_nearClippingPlane;
        m_farClippingPlane = other->m_farClippingPlane;
        m_fov = other->m_fov;

        m_transform->SetPosition(other->m_transform->GetPosition());
        m_transform->SetOrientation(other->m_transform->GetOrientation());
        m_transform->SetScale(other->m_transform->GetScale());
    }

    DirectX::XMVECTOR Camera::GetPosition() const
    {
        return m_transform->GetPosition();
    }

    DirectX::XMMATRIX Camera::GetViewProjectionMatrix() const
    {
        return DirectX::XMMatrixTranspose(GetViewMatrix() * GetProjectionMatrix());
    }

    Ray Camera::ScreenPointToRay(int x, int y) const
    {
        //auto p1 = GetPosition();
        //*
        auto p1 = DirectX::XMVector3Unproject(
            DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0, 0),
            0, 0, 1280, 720, 0, 1,
            GetProjectionMatrix(),
            GetViewMatrix(),
            DirectX::XMMatrixIdentity()
        );
        //*/

        auto p2 = DirectX::XMVector3Unproject(
            DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 1, 0),
            0, 0, 1280, 720, 0, 1,
            GetProjectionMatrix(),
            GetViewMatrix(),
            DirectX::XMMatrixIdentity()
        );

        auto direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(p2, p1));
        
        return { p1, direction };
    }

    DirectX::XMVECTOR Camera::WorldPositionToScreenPoint(DirectX::XMVECTOR position) const
    {
        return DirectX::XMVector3Project(
            position,
            0, 0, 1280, 720, 0, 1,
            GetProjectionMatrix(),
            GetViewMatrix(),
            DirectX::XMMatrixIdentity()
        );
    }

    DirectX::XMVECTOR Camera::WorldDirectionToScreenDirection(DirectX::XMVECTOR direction) const
    {
        // TODO:
        // We couldn't figure out the simple way of doing this projection.
        // The real solution will be an exercise for later.

        const auto p0 = WorldPositionToScreenPoint({ 0, 0, 0, 1 });
        const auto p1 = WorldPositionToScreenPoint(DirectX::XMVectorSetW(direction, 1));
        
        return DirectX::XMVector2Normalize(
            {
                DirectX::XMVectorGetX(p1) - DirectX::XMVectorGetX(p0),
                DirectX::XMVectorGetY(p1) - DirectX::XMVectorGetY(p0),
                0, 0
            });
    }

    void Camera::Move(DirectX::XMVECTOR direction, float distance)
    {
        auto pos = m_transform->GetPosition();

        m_transform->SetPosition(
            DirectX::XMVectorGetX(pos) + DirectX::XMVectorGetX(direction) * distance,
            DirectX::XMVectorGetY(pos) + DirectX::XMVectorGetY(direction) * distance,
            DirectX::XMVectorGetZ(pos) + DirectX::XMVectorGetZ(direction) * distance);
    }

    void Camera::Rotate(float xAxis, float yAxis)
    {
        m_transform->RotateAroundLocalAxisX(xAxis); // local
        m_transform->RotateAroundWorldAxisY(yAxis); // world
    }

    void Camera::SetType(CameraType type)
    {
        m_type = type;
    }

    DirectX::XMMATRIX Camera::GetViewMatrix() const
    {
        return DirectX::XMMatrixLookToLH(
            m_transform->GetPosition(),
            m_transform->GetForward(),
            m_transform->GetUp());
    }

    DirectX::XMMATRIX Camera::GetProjectionMatrix() const
    {
        switch (m_type)
        {
        case CameraType::Orthographic:
            return GetOrthographicProjectionMatrix();
        case CameraType::Perspective:
            return GetPerspectiveProjectionMatrix();
        }

        ENGINE_ASSERT(false, "Unknown CameraType");
        
        return DirectX::XMMatrixIdentity();
    }

    DirectX::XMMATRIX Camera::GetOrthographicProjectionMatrix() const
    {
        return DirectX::XMMatrixOrthographicLH(m_orthoViewWidth, m_orthoViewHeight, m_nearClippingPlane, m_farClippingPlane);
    }

    DirectX::XMMATRIX Camera::GetPerspectiveProjectionMatrix() const
    {
        const float aspectRatio = 16.f / 9.f;

        return DirectX::XMMatrixPerspectiveFovLH(m_fov, aspectRatio, m_nearClippingPlane, m_farClippingPlane);
    }

    void Camera::SetFieldOfView(float fov)
    {
        m_fov = fov;
    }

    void Camera::SetNearClippingPlane(float nearClip)
    {
        m_nearClippingPlane = nearClip;
    }

    void Camera::SetFarClippingPlane(float farClip)
    {
        m_farClippingPlane = farClip;
    }

    void Camera::SetPosition(DirectX::XMVECTOR position)
    {
        m_transform->SetPosition(position);
    }

    void Camera::SetPosition(float x, float y, float z)
    {
        m_transform->SetPosition(x, y, z);
    }

}
