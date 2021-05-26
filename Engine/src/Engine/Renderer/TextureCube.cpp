#include "pch.h"
#include "TextureCube.h"
#include "Graphics.h"
#include "DirectXTK\DDSTextureLoader.h"

namespace Engine {


    TextureCube::TextureCube(Microsoft::WRL::ComPtr<ID3D11Texture2D> right, Microsoft::WRL::ComPtr<ID3D11Texture2D> left, Microsoft::WRL::ComPtr<ID3D11Texture2D> top, Microsoft::WRL::ComPtr<ID3D11Texture2D> bottom, Microsoft::WRL::ComPtr<ID3D11Texture2D> front, Microsoft::WRL::ComPtr<ID3D11Texture2D> back)
    {
        // We copy the width/height etc from one of the input images, 
        // assuming that they are all similar
        D3D11_TEXTURE2D_DESC imageDesc;
        right->GetDesc(&imageDesc);
        const UINT width = imageDesc.Width;
        const UINT height = imageDesc.Height;
        const DXGI_FORMAT format = imageDesc.Format;

        {
            //
            // CREATE THE TEXTURECUBE
            //

            textureDesc_.Width = width;
            textureDesc_.Height = height;
            textureDesc_.MipLevels = 1;
            textureDesc_.ArraySize = 6;
            textureDesc_.Format = format;
            textureDesc_.CPUAccessFlags = 0;
            textureDesc_.SampleDesc.Count = 1;
            textureDesc_.SampleDesc.Quality = 0;
            textureDesc_.Usage = D3D11_USAGE_DEFAULT;
            textureDesc_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            textureDesc_.CPUAccessFlags = 0;
            textureDesc_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

            DX::ThrowIfFailed(Graphics::GetDevice()->CreateTexture2D(&textureDesc_, NULL, &texture_));
        }

        {
            //
            // COPY DATA INTO THE TEXTURECUBE
            //

            Microsoft::WRL::ComPtr<ID3D11Texture2D> images[6]
            {
                right,
                left,
                top,
                bottom,
                front,
                back
            };

            D3D11_BOX sourceRegion = {};
            sourceRegion.left = 0;
            sourceRegion.right = width;
            sourceRegion.top = 0;
            sourceRegion.bottom = height;
            sourceRegion.front = 0;
            sourceRegion.back = 1;

            WITH_GUARDED_GRAPHICS_CONTEXT
            (
                for (int i = 0; i < 6; i++)
                {
                    Graphics::GetContext()->CopySubresourceRegion(texture_.Get(), i, 0, 0, 0, images[i].Get(), 0, &sourceRegion);
                }
            );
        }

        {
            //
            // CREATE THE SHADER_RESOURCE_VIEW
            //

            D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            desc.TextureCube.MipLevels = 1;
            desc.TextureCube.MostDetailedMip = 0;

            DX::ThrowIfFailed(Graphics::GetDevice()->CreateShaderResourceView(texture_.Get(), &desc, &resourceView_));
        }
    }

    TextureCube::TextureCube(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture)
        : texture_(texture)
    {
        texture->GetDesc(&textureDesc_);
        const DXGI_FORMAT format = textureDesc_.Format;

        {
            //
            // CREATE THE SHADER_RESOURCE_VIEW
            //

            D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            desc.TextureCube.MipLevels = 1;
            desc.TextureCube.MostDetailedMip = 0;

            DX::ThrowIfFailed(Graphics::GetDevice()->CreateShaderResourceView(texture_.Get(), &desc, &resourceView_));
        }
    }

    TextureCube::TextureCube(const std::filesystem::path& filepath)
    {
        if (filepath.generic_string().ends_with(".dds"))
        {
            Microsoft::WRL::ComPtr<ID3D11Resource> resource;
            const std::wstring wfilepath = StringToWString(filepath.generic_string());
            WITH_GUARDED_GRAPHICS_CONTEXT(DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(Graphics::GetDevice(), Graphics::GetContext(), wfilepath.c_str(), &resource, &resourceView_)));
            DX::ThrowIfFailed(resource->QueryInterface(IID_ID3D11Texture2D, (void**)&texture_));

            texture_->GetDesc(&textureDesc_);
        }
        else
        {
            // TODO: We only support DDS files for now
            ENGINE_ASSERT(false, "")
        }
    }

    void TextureCube::Bind(int slot) const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->PSSetShaderResources(slot, 1, resourceView_.GetAddressOf()));
    }

}


