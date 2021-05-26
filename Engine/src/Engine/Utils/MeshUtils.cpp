#include "pch.h"
#include "..\Renderer\Mesh.h"
#include "..\Renderer\IndexBuffer.h"
#include "..\Renderer\VertexTypes.h"
#include "..\Renderer\VertexBuffer.h"

namespace Engine 
{

    std::shared_ptr<Mesh> CreateQuadMesh()
    {
        // TODO: Implement and use by GizmoLayer::FindAndRenderColliders()

        return nullptr;
    }

    std::shared_ptr<Mesh> CreateSkyboxMesh()
    {
        std::vector<float> vertexData = 
        {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        std::vector<IndexBuffer::IndexType> indexData =
            {
                0, 1, 2,
                3, 4, 5,

                6, 7, 8,
                9, 10, 11,

                12, 13, 14,
                15, 16, 17,

                18, 19, 20,
                21, 22, 23,

                24, 25, 26,
                27, 28, 29,

                30, 31, 32,
                33, 34, 35
            };

        const auto vertices = std::make_shared<VertexBuffer>(vertexData, GetVertexTypeSize(VertexType::Minimal));
        const auto indices = std::make_shared<IndexBuffer>(indexData);
        auto mesh = std::make_shared<Mesh>(vertices, indices, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        return mesh;
    }

}
