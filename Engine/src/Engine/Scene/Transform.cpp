#include "pch.h"
#include "Transform.h"
#include "..\EngineAssert.h"
#include <numbers>

namespace Engine
{

    std::shared_ptr<Engine::Camera> Transform::FindCamera(const std::string& name)
    {
        if (HasCamera() && m_name == name)
        {
            return m_camera;
        }

        for (auto transform : m_children)
        {
            if (const auto camera = transform->FindCamera(name))
            {
                return camera;
            }
        }

        return nullptr;
    }

    Engine::EulerAngles Transform::GetEulerAngles()
    {
        if (m_eulerAnglesNeedsUpdate)
        {
            m_eulerAngles.SetFromQuaternion(m_orientation);
            m_eulerAnglesNeedsUpdate = false;
        }

        return m_eulerAngles;
    }

    void Transform::SetEulerAngles(EulerAngles angles)
    {
        m_eulerAngles = angles;
        m_orientation = DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, angles.z);
    }

    void Transform::RotateAroundLocalAxis(DirectX::XMVECTOR axis, float angle)
    {
        auto rotation = DirectX::XMQuaternionRotationNormal(axis, angle);
        m_orientation = DirectX::XMQuaternionMultiply(rotation, m_orientation);
        m_eulerAnglesNeedsUpdate = true;
    }

    void Transform::RotateAroundWorldAxis(DirectX::XMVECTOR axis, float angle)
    {
        auto rotation = DirectX::XMQuaternionRotationNormal(axis, angle);

        if (HasParent())
        {
            //
            // The formula is:
            // local orientation * inverse of parent's world orientation * rotation * parent's world orientation
            //

            DirectX::XMVECTOR derivedOrientation = m_parent->GetWorldOrientation();

            m_orientation = DirectX::XMQuaternionMultiply(
                DirectX::XMQuaternionMultiply(
                    DirectX::XMQuaternionMultiply(
                        m_orientation,
                        DirectX::XMQuaternionInverse(derivedOrientation)),
                    rotation),
                derivedOrientation);
        }
        else
        {
            m_orientation = DirectX::XMQuaternionMultiply(m_orientation, rotation);
        }

        m_eulerAnglesNeedsUpdate = true;
    }

    void Transform::AttachChildToParent(std::shared_ptr<Transform> parent, std::shared_ptr<Transform> child, bool preserveWorldTransform)
    {
        if (preserveWorldTransform)
        {
            auto inverseParentMatrix = DirectX::XMMatrixInverse(nullptr, parent->GetWorldModelMatrix());
            auto localMatrix = child->GetLocalModelMatrix();

            auto m = DirectX::XMMatrixTranspose(
                DirectX::XMMatrixMultiply(inverseParentMatrix, localMatrix));

            bool result = DirectX::XMMatrixDecompose(
                &child->m_scale,
                &child->m_orientation,
                &child->m_position,
                m);

            ENGINE_ASSERT(result, "Matrix decomposition failed");
        }

        child->m_parent = parent;
        parent->m_children.push_back(child);
    }

    void Transform::DetachChildFromParent(std::shared_ptr<Transform> parent, std::shared_ptr<Transform> child, bool preserveWorldTransform)
    {
        ENGINE_ASSERT(child->GetParent() == parent, "Wrong parent");
        
        if (preserveWorldTransform)
        {
            bool result = DirectX::XMMatrixDecompose(
                &child->m_scale,
                &child->m_orientation,
                &child->m_position,
                DirectX::XMMatrixTranspose(child->GetWorldModelMatrix()));

            ENGINE_ASSERT(result, "Matrix decomposition failed");
        }

        parent->m_children.erase(std::remove(parent->m_children.begin(), parent->m_children.end(), child), parent->m_children.end());
        child->m_parent = nullptr;
    }

    DirectX::XMMATRIX Transform::GetLocalModelMatrix()
    {
        auto matrix = DirectX::XMMatrixTranspose(
            DirectX::XMMatrixScalingFromVector(m_scale) *
            DirectX::XMMatrixRotationQuaternion(m_orientation) *
            DirectX::XMMatrixTranslationFromVector(m_position)
        );

        return matrix;
    }

    DirectX::XMMATRIX Transform::GetWorldModelMatrix()
    {
        if (HasParent())
        {
            return m_parent->GetWorldModelMatrix() * GetLocalModelMatrix();
        }
        else
        {
            return GetLocalModelMatrix();
        }
    }

    DirectX::XMVECTOR Transform::GetWorldOrientation()
    {
        DirectX::XMVECTOR scale;
        DirectX::XMVECTOR orientation;
        DirectX::XMVECTOR position;

        bool result = DirectX::XMMatrixDecompose(&scale, &orientation, &position, GetWorldModelMatrix());
        ENGINE_ASSERT(result, "Matrix decomposition failed");
        return orientation;
    }

    DirectX::XMVECTOR Transform::GetWorldPosition()
    {
        return InverseTransformPoint(m_position);
    }

    void Transform::AddAllCamerasToVector(std::vector<std::shared_ptr<Camera>>& cameras)
    {
        if (HasCamera())
        {
            cameras.push_back(m_camera);
        }

        for (auto child : m_children)
        {
            child->AddAllCamerasToVector(cameras);
        }
    }

    void EulerAngles::SetFromQuaternion(const DirectX::XMVECTOR& quaternion)
    {
        struct Quat { float w, x, y, z; };

        DirectX::XMVECTOR normalized = DirectX::XMQuaternionNormalize(quaternion);

        Quat q
        {
            .w = DirectX::XMVectorGetW(normalized),
            .x = DirectX::XMVectorGetX(normalized),
            .y = DirectX::XMVectorGetY(normalized),
            .z = DirectX::XMVectorGetZ(normalized),
        };

        // roll (x-axis rotation)
        float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
        float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
        x = std::atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        float sinp = 2 * (q.w * q.y - q.z * q.x);
        if (std::abs(sinp) >= 1)
            y = static_cast<float>(std::copysign(std::numbers::pi / 2, sinp)); // use 90 degrees if out of range
        else
            y = std::asin(sinp);

        // yaw (z-axis rotation)
        float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
        float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
        z = std::atan2(siny_cosp, cosy_cosp);
    }

}
