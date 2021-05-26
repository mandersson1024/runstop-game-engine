#pragma once
#include "ConstantBuffer.h"

namespace Engine
{

    class SceneConstantBufferVS : public ConstantBuffer
    {
    public:
        SceneConstantBufferVS()
            : ConstantBuffer(ConstantBuffer::Type::VertexShader)
        {
            Initialize();
        }

        void SetCameraViewProjectionMatrix(DirectX::XMMATRIX matrix) { m_data.cameraViewProjectionMatrix = matrix; }
        void SetLightViewProjectionMatrix(DirectX::XMMATRIX matrix) { m_data.lightViewProjectionMatrix = matrix; }
        
        void SetLightPosition(DirectX::XMFLOAT3A pos) { m_data.lightPosition = pos; }
        void SetLightPosition(DirectX::XMVECTOR pos) { DirectX::XMStoreFloat3(&m_data.lightPosition, pos); }

    private:
        struct Data
        {
            DirectX::XMMATRIX cameraViewProjectionMatrix = DirectX::XMMatrixIdentity();
            DirectX::XMMATRIX lightViewProjectionMatrix = DirectX::XMMatrixIdentity();
            DirectX::XMFLOAT3A lightPosition = { 0, 0, 0 };
        };

    protected:
        uint32_t GetByteWidth() const override { return sizeof(Data); }
        void* GetData() { return reinterpret_cast<void*>(&m_data); }

    private:
        Data m_data;
    };
};

