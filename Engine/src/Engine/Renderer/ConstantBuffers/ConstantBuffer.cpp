#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine/Renderer/Graphics.h"

namespace Engine
{

    ConstantBuffer::ConstantBuffer(Type type)
        : type_(type)
    {
    }

    void ConstantBuffer::Initialize()
    {
        D3D11_BUFFER_DESC desc
        {
            .ByteWidth = GetByteWidth(),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0,
        };

        D3D11_SUBRESOURCE_DATA subresource
        {
            .pSysMem = GetData()
        };

        DX::AssertIfFailed(Graphics::GetDevice()->CreateBuffer(&desc, &subresource, &buffer_));
    }

    void ConstantBuffer::Bind(uint32_t slot)
    {
        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            Graphics::GetContext()->UpdateSubresource(buffer_.Get(), 0, 0, GetData(), 0, 0);

            if (type_ == Type::VertexShader)
            {
                Graphics::GetContext()->VSSetConstantBuffers(slot, 1, buffer_.GetAddressOf());
            }

            if (type_ == Type::PixelShader)
            {
                Graphics::GetContext()->PSSetConstantBuffers(slot, 1, buffer_.GetAddressOf());
            }
        );
    }

}

