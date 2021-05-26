#pragma once
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>

namespace Engine
{
	class IndexBuffer
	{
	public:
		using IndexType = uint16_t;

		IndexBuffer(const std::vector<IndexType>& indices);

		uint32_t GetCount() const { return count_;  }
		void Bind() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
		uint32_t count_;
	};
}

