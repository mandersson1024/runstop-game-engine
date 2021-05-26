#pragma once
#include "ConstantBuffer.h"

namespace Engine
{

    class ModelConstantBufferPS : public ConstantBuffer
    {
    public:
        ModelConstantBufferPS()
            : ConstantBuffer(ConstantBuffer::Type::PixelShader)
        {
            Initialize();
        }

        void SetBaseColorTint(DirectX::XMFLOAT3 tint) { m_data.baseColorTint = tint; }
        void SetRoughnessRange(DirectX::XMFLOAT2 range) { m_data.roughnessRange = range; }
        void SetMetallicRange(DirectX::XMFLOAT2 range) { m_data.metallicRange = range; }
        void SetAmbientOcclusionRange(DirectX::XMFLOAT2 range) { m_data.ambientOcclusionRange = range; }
        void SetDebugInvertColors(bool invert) { m_data.debugInvertColors = invert; }

    private:
        struct Data
        {
            DirectX::XMFLOAT3 baseColorTint = { 1, 1, 1 };
            float _pad_0;
            DirectX::XMFLOAT2 roughnessRange = { 0, 1 };
            DirectX::XMFLOAT2 metallicRange = { 0, 1 };
            DirectX::XMFLOAT2 ambientOcclusionRange = { 0, 1 };
            bool debugInvertColors = false;
            float _pad_1;
        };

    protected:
        uint32_t GetByteWidth() const override { return sizeof(Data); }
        void* GetData() { return reinterpret_cast<void*>(&m_data); }

    private:
        Data m_data;
    };

}

