#pragma once
#include "ConstantBuffer.h"

namespace Engine
{

    class SceneConstantBufferPS : public ConstantBuffer
    {
    public:
        SceneConstantBufferPS()
            : ConstantBuffer(ConstantBuffer::Type::PixelShader)
        {
            Initialize();
        }

        void SetCameraPosition(DirectX::XMFLOAT3A position) { m_data.cameraPosition = position; }
        void SetCameraPosition(DirectX::XMVECTOR position) { DirectX::XMStoreFloat3(&m_data.cameraPosition, position); }

        void SetLightPosition(DirectX::XMFLOAT3A position) { m_data.lightPosition = position; }
        void SetLightPosition(DirectX::XMVECTOR position) { DirectX::XMStoreFloat3(&m_data.lightPosition, position); }

        void SetLightColor(DirectX::XMFLOAT3A color) { m_data.lightColor = color; }
        void SetLightColor(DirectX::XMVECTOR color) { DirectX::XMStoreFloat3(&m_data.lightColor, color); }

        void SetLightDirection(DirectX::XMFLOAT3 direction) { m_data.lightDirection = direction; }
        void SetLightDirection(DirectX::XMVECTOR direction) { DirectX::XMStoreFloat3(&m_data.lightDirection, direction); }

        void SetDebugSkyboxMipMapLevel(float level) { m_data.debugSkyboxMipMapLevel = level; }
        
        void SetDebugIgnoreLightSources(bool b) { m_data.debugIgnoreLightSources = b; }
        void SetDebugIgnoreAmbientIBL(bool b) { m_data.debugIgnoreAmbientIBL = b; }
        void SetDebugIgnoreSpecularIBL(bool b) { m_data.debugIgnoreSpecularIBL = b; }

    private:
        
        struct Data
        {
            DirectX::XMFLOAT3A cameraPosition = { 0, 0, 0 };
            DirectX::XMFLOAT3A lightPosition = { -3, 3, 0 };
            DirectX::XMFLOAT3A lightColor = { 20, 20, 20 };
            DirectX::XMFLOAT3 lightDirection = { 0.5f, -0.8f, 1.f };
            float debugSkyboxMipMapLevel = 0;
            BOOL debugIgnoreLightSources;
            BOOL debugIgnoreAmbientIBL;
            BOOL debugIgnoreSpecularIBL;
        };

    protected:
        uint32_t GetByteWidth() const override { return sizeof(Data); }
        void* GetData() { return reinterpret_cast<void*>(&m_data); }

    private:
        Data m_data;
    };
};

