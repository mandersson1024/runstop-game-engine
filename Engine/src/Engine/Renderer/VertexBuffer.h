#pragma once
#include <wrl/client.h>
#include <vector>
#include <d3d11.h>

namespace Engine
{

    class VertexBuffer
    {
    public:
        VertexBuffer(const std::vector<float>& vertexData, uint32_t stride);

        void Bind() const;

    private:
        uint32_t stride_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
    };
}

