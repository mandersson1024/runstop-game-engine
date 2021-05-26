#pragma once

namespace Engine
{

    class ConstantBuffer
    {
    public:
        enum class Type
        {
            VertexShader,
            PixelShader,
        };

    public:
        ConstantBuffer(Type type);
        virtual ~ConstantBuffer() {}

        void Bind(uint32_t slot);

    protected:
        void Initialize();
        virtual uint32_t GetByteWidth() const = 0;
        virtual void* GetData() = 0;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
        Type type_;
    };

}
