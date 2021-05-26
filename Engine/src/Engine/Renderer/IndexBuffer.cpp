#include "pch.h"
#include "IndexBuffer.h"
#include "Graphics.h"

namespace Engine
{

	IndexBuffer::IndexBuffer(const std::vector<IndexType>& indices) 
		: count_(static_cast<uint32_t>(indices.size()))
	{
		D3D11_BUFFER_DESC bufferDesc
		{
			.ByteWidth = count_ * sizeof(IndexType),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
			.StructureByteStride = sizeof(IndexType),
		};

		D3D11_SUBRESOURCE_DATA subresourceData
		{
			.pSysMem = indices.data(),
			.SysMemPitch = 0, // has no meaning for index buffers
			.SysMemSlicePitch = 0, // has no meaning for index buffers
		};

		DX::ThrowIfFailed(Graphics::GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &buffer_));
	}

	void IndexBuffer::Bind() const
	{
		WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->IASetIndexBuffer(buffer_.Get(), DXGI_FORMAT_R16_UINT, 0));
	}

}