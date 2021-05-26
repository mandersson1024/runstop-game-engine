#pragma once
#include <memory>
#include <DirectXMath.h>

namespace Engine
{
    class Transform;

    struct Ray
    {
        DirectX::XMVECTOR origin;
        DirectX::XMVECTOR direction;
    };

    struct RaycastHit
    {
        bool valid;
        DirectX::XMVECTOR point;
        DirectX::XMVECTOR normal;
        float distance;
    };

    struct Triangle
    {
        DirectX::XMVECTOR p1;
        DirectX::XMVECTOR p2;
        DirectX::XMVECTOR p3;
    };

    class BoxCollider
    {
    public:
        void SetTransform(std::shared_ptr<Transform> transform) { m_transform = transform; }
        std::shared_ptr<Transform> GetTransform() const { return m_transform; }

        void SetSize(float x, float y, float z) 
        { 
            m_size[0] = x;
            m_size[1] = y;
            m_size[2] = z;
        }

        float GetSizeX() const { return m_size[0]; }
        float GetSizeY() const { return m_size[1]; }
        float GetSizeZ() const { return m_size[2]; }

        void SetOffset(float x, float y, float z) 
        { 
            m_offset[0] = x; 
            m_offset[1] = y; 
            m_offset[2] = z; 
        }
        
        float GetOffsetX() const { return m_offset[0]; }
        float GetOffsetY() const { return m_offset[1]; }
        float GetOffsetZ() const { return m_offset[2]; }

        void GetVertices(DirectX::XMVECTOR out_verts[8]);

        RaycastHit Raycast(const Ray& ray);

    private:
        float m_size[3] = { 1, 1, 1 };
        float m_offset[3] = { 0, 0, 0 };

        std::shared_ptr<Transform> m_transform;
    };

    class PlaneCollider
    {
    public:
        PlaneCollider()
            : normal_{}
            , point_{}
        {
        }
        
        PlaneCollider(DirectX::XMVECTOR normal, DirectX::XMVECTOR point)
            : normal_(normal)
            , point_(point)
        {
        }

        DirectX::XMVECTOR GetPoint() const { return point_; }
        DirectX::XMVECTOR GetNormal() const { return normal_; }

        RaycastHit Raycast(const Ray& ray);

    private:
        DirectX::XMVECTOR normal_;
        DirectX::XMVECTOR point_;
    };

}

