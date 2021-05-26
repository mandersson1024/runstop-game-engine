#include "pch.h"
#include "Mesh.h"
#include "..\Renderer\Graphics.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Engine
{

    Mesh::Mesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, D3D_PRIMITIVE_TOPOLOGY topology, MeshInfo info)
        : m_vertexBuffer(vertexBuffer)
        , m_indexBuffer(indexBuffer)
        , m_topology(topology)
        , m_info(info)
    {
    }

    int Mesh::GetTriangleCount() const
    {
        switch (m_topology)
        {
        case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
        case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
        case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
        case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
            return m_indexBuffer->GetCount() / 3;
        default:
            return 0;
        }
    }

    void Mesh::Bind() const
    {
        m_vertexBuffer->Bind();
        m_indexBuffer->Bind();

        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->IASetPrimitiveTopology(m_topology));
    }
}

