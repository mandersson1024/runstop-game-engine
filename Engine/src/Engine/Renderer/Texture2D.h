#pragma once
#include <d3d11.h>
#include <memory>

namespace Engine
{

    class RawTexture;

    class Texture2D
    {
    public:
        static std::shared_ptr<Texture2D> CreateFromRawTexture(const RawTexture& raw, bool mimpmap);
        static std::shared_ptr<Texture2D> CreateFromPixelData(int width, int height, int pixelByteSize, void* data, bool mipmap);
        static std::shared_ptr<Texture2D> CreateFromGPUTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture);

        int GetWidth() const { return width_; }
        int GetHeight() const { return height_; }
        
        ID3D11Texture2D* GetTexture() const { return texture_.Get(); }
        ID3D11ShaderResourceView* GetResourceView() const { return resourceView_.Get(); }

        void Bind(int slot) const;
        void BindCS(int slot) const;

    private:
        void Create(uint32_t rowPitch, void* data, DXGI_FORMAT format, bool mipmap);

    private:
        int width_ = 0;
        int height_ = 0;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView_;
    };

}

