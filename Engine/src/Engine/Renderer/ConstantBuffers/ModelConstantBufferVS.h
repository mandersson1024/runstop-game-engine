#pragma once
#include "ConstantBuffer.h"

namespace Engine
{

    class ModelConstantBufferVS : public ConstantBuffer
    {
    public:
        ModelConstantBufferVS()
            : ConstantBuffer(ConstantBuffer::Type::VertexShader)
        {
            Initialize();
        }

        void SetModelMatrix(DirectX::XMMATRIX modelMatrix) { m_data.modelMatrix = modelMatrix; }
        void SetUVRepeat(float repeat) { m_data.uvRepeat = repeat; }

    private:
        struct Data
        {
            DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixIdentity();
            float uvRepeat = 200;
        };

    protected:
        uint32_t GetByteWidth() const override { return sizeof(Data); }
        void* GetData() { return reinterpret_cast<void*>(&m_data); }

    private:
        Data m_data;
    };

}

