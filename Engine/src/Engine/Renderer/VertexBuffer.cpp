#include "pch.h"
#include "VertexBuffer.h"
#include "Graphics.h"

namespace Engine
{

    VertexBuffer::VertexBuffer(const std::vector<float>& vertexData, uint32_t stride) 
        : stride_ (stride)
    {
        D3D11_BUFFER_DESC bufferDesc
        {
            .ByteWidth = static_cast<UINT>(vertexData.size()) * sizeof(float),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = stride_,
        };

        D3D11_SUBRESOURCE_DATA subresourceData
        {
            .pSysMem = vertexData.data(),
            .SysMemPitch = 0, // has no meaning for vertex buffers
            .SysMemSlicePitch = 0, // has no meaning for vertex buffers
        };

        auto result = Graphics::GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &buffer_);

        DX::ThrowIfFailed(result);
    }

    void VertexBuffer::Bind() const
    {
        // TODO:
        // This only draws one vertex buffer, while it is possible to draw several
        // at once. Therefore, this should be handled at a higher lever than this.
        const UINT strides[] = { stride_ };
        const UINT offsets[] = { 0 };
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->IASetVertexBuffers(0, 1, buffer_.GetAddressOf(), strides, offsets));
    }

}

