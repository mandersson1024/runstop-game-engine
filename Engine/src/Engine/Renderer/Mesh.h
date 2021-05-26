#pragma once
#include <string>
#include <memory>
#include <d3dcommon.h>

namespace Engine
{

    class VertexBuffer;
    class IndexBuffer;

    struct MeshInfo
    {
        std::string name = "[undefined]";
        uint32_t triangleCount = 0;
    };

    class Mesh
    {
    public:
        Mesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, D3D_PRIMITIVE_TOPOLOGY topology, MeshInfo info = {});

        std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_indexBuffer;  }
        int GetTriangleCount() const;
        void Bind() const;
        MeshInfo& GetInfo() { return m_info; }

    private:
        std::shared_ptr<VertexBuffer> m_vertexBuffer;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        D3D_PRIMITIVE_TOPOLOGY m_topology;
        MeshInfo m_info;
    };

}


