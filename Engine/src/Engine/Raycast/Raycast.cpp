#include "pch.h"
#include "Raycast.h"
#include <DirectXCollision.h>
#include "..\Scene\Transform.h"

namespace Engine
{

    bool CalculateIntersection(const Ray& ray, const std::vector<Triangle>& triangles, float* out_distance, DirectX::XMVECTOR* out_normal)
    {
        *out_distance = std::numeric_limits<float>::max();
        bool hit = false;

        for (const auto& triangle : triangles)
        {
            float distance;
            if (DirectX::TriangleTests::Intersects(ray.origin, ray.direction, triangle.p1, triangle.p2, triangle.p3, distance))
            {
                hit = true;

                if (distance < *out_distance)
                {
                    *out_distance = distance;
                    *out_normal = DirectX::XMVector3Cross(
                        DirectX::XMVectorSubtract(triangle.p2, triangle.p1),
                        DirectX::XMVectorSubtract(triangle.p3, triangle.p1));
                }
            }
        }

        return hit;
    }

    void BoxCollider::GetVertices(DirectX::XMVECTOR out_verts[8])
    {
        auto matrix = DirectX::XMMatrixTranspose(m_transform->GetWorldModelMatrix());

        float sx = m_size[0];
        float sy = m_size[1];
        
        float ox = m_offset[0];
        float oy = m_offset[1];
        float oz = m_offset[2];

        out_verts[0] = DirectX::XMVector4Transform({ -0.5f * sx + ox, -0.5f * sy + oy, -0.5f + oz, 1 }, matrix);
        out_verts[1] = DirectX::XMVector4Transform({ -0.5f * sx + ox,  0.5f * sy + oy, -0.5f + oz, 1 }, matrix);
        out_verts[2] = DirectX::XMVector4Transform({  0.5f * sx + ox,  0.5f * sy + oy, -0.5f + oz, 1 }, matrix);
        out_verts[3] = DirectX::XMVector4Transform({  0.5f * sx + ox, -0.5f * sy + oy, -0.5f + oz, 1 }, matrix);
        out_verts[4] = DirectX::XMVector4Transform({ -0.5f * sx + ox, -0.5f * sy + oy,  0.5f + oz, 1 }, matrix);
        out_verts[5] = DirectX::XMVector4Transform({ -0.5f * sx + ox,  0.5f * sy + oy,  0.5f + oz, 1 }, matrix);
        out_verts[6] = DirectX::XMVector4Transform({  0.5f * sx + ox,  0.5f * sy + oy,  0.5f + oz, 1 }, matrix);
        out_verts[7] = DirectX::XMVector4Transform({  0.5f * sx + ox, -0.5f * sy + oy,  0.5f + oz, 1 }, matrix);
    }

    RaycastHit BoxCollider::Raycast(const Ray& ray)
    {
        DirectX::XMVECTOR verts[8];
        GetVertices(verts);

        RaycastHit hit
        {
            .valid = false,
        };

        float distance;
        DirectX::XMVECTOR normal;
        hit.valid = CalculateIntersection(ray,
            {
                // front
                { verts[0], verts[1], verts[2] },
                { verts[0], verts[2], verts[3] },

                // back
                { verts[4], verts[6], verts[5] },
                { verts[4], verts[7], verts[6] },

                // left
                { verts[0], verts[5], verts[1] },
                { verts[0], verts[4], verts[5] },

                // right
                { verts[3], verts[2], verts[6] },
                { verts[3], verts[6], verts[7] },

                // top
                { verts[1], verts[6], verts[2] },
                { verts[1], verts[5], verts[6] },

                // bottom
                { verts[0], verts[3], verts[7] },
                { verts[0], verts[7], verts[4] },
            },
            &distance,
            &normal);

        if (hit.valid)
        {
            hit.point = DirectX::XMVectorMultiplyAdd(ray.direction, { distance, distance, distance, 1.f }, ray.origin);
            hit.normal = normal;
            hit.distance = distance;
        }

        return hit;
    }

    /*
    Engine::RaycastHit PlaneCollider::Raycast(const Ray& ray)
    {
        const auto plane = DirectX::XMPlaneFromPointNormal(m_point, m_normal);
        const auto hitpoint = DirectX::XMPlaneIntersectLine(plane, ray.origin, DirectX::XMVectorMultiplyAdd(ray.direction, { 1, 1, 1, 0 }, ray.origin));

        if (DirectX::XMVector4Equal(hitpoint, DirectX::XMVectorSplatQNaN()))
        {
            return RaycastHit
            {
                .valid = false
            };
        }
        else
        {
            float distance = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(hitpoint, ray.origin));
            ENGINE_LOG("distance: {}", distance);

            if (distance <= 0)
            {
                return RaycastHit
                {
                    .valid = false
                };
            }
            else
            {
                return RaycastHit
                {
                    .valid = true,
                    .point = DirectX::XMVectorAdd(ray.origin, DirectX::XMVectorScale(ray.direction, distance)),
                    .normal = m_normal,
                    .distance = distance
                };
            }
        }
    }
    */

    Engine::RaycastHit PlaneCollider::Raycast(const Ray& ray)
    {
        const auto plane = DirectX::XMPlaneFromPointNormal(point_, normal_);
        bool normalsAreInTheSameDirection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal_, ray.direction)) > 0;

        if (normalsAreInTheSameDirection)
        {
            return RaycastHit
            {
                .valid = false
            };
        }

        const auto hitpoint = DirectX::XMPlaneIntersectLine(plane, ray.origin, DirectX::XMVectorMultiplyAdd(ray.direction, { 100, 100, 100, 0 }, ray.origin));

        if (DirectX::XMVector3IsNaN(hitpoint))
        {
            return RaycastHit
            {
                .valid = false
            };
        }

        float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(hitpoint, ray.origin)));
        ENGINE_LOG("distance: {}", distance);

        return RaycastHit
        {
            .valid = true,
            .point = DirectX::XMVectorAdd(ray.origin, DirectX::XMVectorScale(ray.direction, distance)),
            .normal = normal_,
            .distance = distance
        };
    }

}

