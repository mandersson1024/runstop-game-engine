#pragma once
#include <vector>
#include <string>
#include <memory>

namespace Engine
{
    class GameObject;
    class Camera;
    class LightSource;
    class BoxCollider;

    struct EulerAngles
    {
        float x = 0;
        float y = 0;
        float z = 0;

        void SetFromQuaternion(const DirectX::XMVECTOR& quaternion);
    };

    class Transform
    {
    public:
        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        bool HasGameObject() const { return m_gameObject != nullptr; }
        std::shared_ptr<GameObject> GetGameObject() const { return m_gameObject; }
        void SetGameObject(std::shared_ptr<GameObject> gameObject) { m_gameObject = gameObject; }

        bool HasParent() const { return m_parent != nullptr; }
        bool HasChildren() const { return !m_children.empty(); }

        bool HasCamera() const { return m_camera != nullptr; }
        std::shared_ptr<Camera> GetCamera() { return m_camera;  }
        void SetCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
        std::shared_ptr<Camera> FindCamera(const std::string& name);

        bool HasLightSource() const { return m_lightSource != nullptr; }
        std::shared_ptr<LightSource> GetLightSource() { return m_lightSource;  }
        void SetLightSource(std::shared_ptr<LightSource> source) { m_lightSource = source; }

        bool HasCollider() const { return m_collider != nullptr; }
        std::shared_ptr<BoxCollider> GetCollider() { return m_collider; }
        void SetCollider(std::shared_ptr<BoxCollider> target) { m_collider = target; }

        const DirectX::XMVECTOR& GetPosition() const { return m_position; }
        void SetPosition(DirectX::XMVECTOR position) { m_position = position; }
        void SetPosition(float x, float y, float z) { m_position = { x, y, z, 0 }; }

        const DirectX::XMVECTOR& GetScale() const { return m_scale; }
        void SetScale(DirectX::XMVECTOR scale) { m_scale = scale; }
        void SetScale(float x, float y, float z) { m_scale = { x, y, z, 0 }; }

        const DirectX::XMVECTOR& GetOrientation() const { return m_orientation; }
        void SetOrientation(DirectX::XMVECTOR orientation) 
        { 
            m_orientation = orientation; 
            m_eulerAnglesNeedsUpdate = true;
        }

        std::shared_ptr<Transform> FindChildWithName(const std::string& name) const
        {
            for (auto child : m_children)
            {
                if (child->HasGameObject())
                {
                    if (child->GetName() == name)
                    {
                        return child;
                    }
                }
            }

            return nullptr;
        }

        EulerAngles GetEulerAngles();

        DirectX::XMVECTOR GetForward() { return DirectX::XMVector4Transform({ 0, 0, 1, 0 }, DirectX::XMMatrixRotationQuaternion(m_orientation)); }
        DirectX::XMVECTOR GetRight() { return DirectX::XMVector4Transform({ 1, 0, 0, 0 }, DirectX::XMMatrixRotationQuaternion(m_orientation)); }
        DirectX::XMVECTOR GetUp() { return DirectX::XMVector4Transform({ 0, 1, 0, 0 }, DirectX::XMMatrixRotationQuaternion(m_orientation)); }

        void SetEulerAngles(EulerAngles angles);
        void RotateAroundWorldAxis(DirectX::XMVECTOR axis, float angle);
        void RotateAroundLocalAxis(DirectX::XMVECTOR axis, float angle);

        void RotateAroundWorldAxisX(float angle) { RotateAroundWorldAxis({ 1, 0, 0, 0 }, angle); }
        void RotateAroundWorldAxisY(float angle) { RotateAroundWorldAxis({ 0, 1, 0, 0 }, angle); }
        void RotateAroundWorldAxisZ(float angle) { RotateAroundWorldAxis({ 0, 0, 1, 0 }, angle); }

        void RotateAroundLocalAxisX(float angle) { RotateAroundLocalAxis({ 1, 0, 0, 0 }, angle); }
        void RotateAroundLocalAxisY(float angle) { RotateAroundLocalAxis({ 0, 1, 0, 0 }, angle); }
        void RotateAroundLocalAxisZ(float angle) { RotateAroundLocalAxis({ 0, 0, 1, 0 }, angle); }

        std::shared_ptr<Transform> GetParent() const { return m_parent; }
        const std::vector<std::shared_ptr<Transform>>& GetChildren() const { return m_children; }
        
        static void AttachChildToParent(std::shared_ptr<Transform> parent, std::shared_ptr<Transform> child, bool preserveWorldTransform);
        static void DetachChildFromParent(std::shared_ptr<Transform> parent, std::shared_ptr<Transform> child, bool preserveWorldTransform);

        void Clear()
        {
            m_gameObject = nullptr;
            m_lightSource = nullptr;
            m_camera = nullptr;
            m_parent = nullptr;
            m_collider = nullptr;

            for (auto child : m_children)
            {
                child->Clear();
            }

            m_children.clear();
        }

        DirectX::XMMATRIX GetLocalModelMatrix();
        DirectX::XMMATRIX GetWorldModelMatrix();
        DirectX::XMVECTOR GetWorldOrientation();
        DirectX::XMVECTOR GetWorldPosition();

        DirectX::XMVECTOR TransformPoint(DirectX::XMVECTOR point)
        {
            // Transforms a position from local space to world space
            const auto matrix = DirectX::XMMatrixTranspose(GetWorldModelMatrix());
            return DirectX::XMVector4Transform(point, matrix);
        }

        DirectX::XMVECTOR InverseTransformPoint(DirectX::XMVECTOR point)
        {
            // Transforms a position from world space to local space
            const auto matrix = DirectX::XMMatrixTranspose(GetWorldModelMatrix());
            return DirectX::XMVector4Transform(point, DirectX::XMMatrixInverse(nullptr, matrix));
        }

        /*
        DirectX::XMVECTOR TransformDirection(DirectX::XMVECTOR direction);
        */
        
        DirectX::XMVECTOR InverseTransformDirection(DirectX::XMVECTOR direction)
        {
            // Transforms a direction from world space to local space
            const auto matrix = DirectX::XMMatrixTranspose(GetWorldModelMatrix());
            return DirectX::XMVector3TransformNormal(direction, DirectX::XMMatrixInverse(nullptr, matrix));
        }

        void AddAllCamerasToVector(std::vector<std::shared_ptr<Camera>>& cameras);
    
    private:
        std::string m_name = "[UNDEFINED]";
        
        DirectX::XMVECTOR m_position = { 0, 0, 0, 1 };
        DirectX::XMVECTOR m_scale = { 1, 1, 1, 0 };
        DirectX::XMVECTOR m_orientation = DirectX::XMQuaternionIdentity();

        // Euler angles are unstable, so we keep a copy of them so they
        // don't flicker around if we want to work with them (like in the Editor)
        EulerAngles m_eulerAngles{};
        bool m_eulerAnglesNeedsUpdate = false;

        std::shared_ptr<Transform> m_parent;
        std::vector<std::shared_ptr<Transform>> m_children;
        
        // Components (allowed to be nullptr/Optional)
        std::shared_ptr<GameObject> m_gameObject;
        std::shared_ptr<Camera> m_camera;
        std::shared_ptr<LightSource> m_lightSource;
        std::shared_ptr<BoxCollider> m_collider;
    };

}

