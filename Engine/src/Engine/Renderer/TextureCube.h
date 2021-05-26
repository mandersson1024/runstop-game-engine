#pragma once
#include <string>
#include <filesystem>
#include <d3d11.h>
#include <wrl.h>

namespace Engine 
{

    class TextureCube 
    {
    public:

        TextureCube(Microsoft::WRL::ComPtr<ID3D11Texture2D> right, Microsoft::WRL::ComPtr<ID3D11Texture2D> left, Microsoft::WRL::ComPtr<ID3D11Texture2D> top, Microsoft::WRL::ComPtr<ID3D11Texture2D> bottom, Microsoft::WRL::ComPtr<ID3D11Texture2D> front, Microsoft::WRL::ComPtr<ID3D11Texture2D> back);
        TextureCube(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture);
        TextureCube(const std::filesystem::path& filepath);

        void Bind(int slot) const;

        uint16_t GetNumMipLevels() const { return textureDesc_.MipLevels; }

    private:
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceView_;
        D3D11_TEXTURE2D_DESC textureDesc_ = {};
    };

}
