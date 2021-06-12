#include "pch.h"
#include "Texture2D.h"
#include "Graphics.h"
#include "..\Assets\RawTexture.h"

namespace Engine
{

    std::shared_ptr<Texture2D> Texture2D::CreateFromPixelData(int width, int height, int pixelByteSize, void* data, bool mipmap)
    {
        auto t = std::make_shared<Texture2D>();
        t->width_ = width;
        t->height_ = height;
        t->Create(pixelByteSize * width, data, DXGI_FORMAT_R8G8B8A8_UNORM, mipmap);
        return t;
    }

    std::shared_ptr<Texture2D> Texture2D::CreateFromGPUTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture)
    {
        auto t = std::make_shared<Texture2D>();

        //
        // GET THE FORMAT FROM THE TEXTURE
        //

        D3D11_TEXTURE2D_DESC textureDesc;
        t->texture_->GetDesc(&textureDesc);

        t->width_ = textureDesc.Width;
        t->height_ = textureDesc.Height;

        //
        // CREATE THE SHADER_RESOURCE_VIEW
        //

        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = textureDesc.Format;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MostDetailedMip = 0;
        desc.Texture2D.MipLevels = 1;
        DX::AssertIfFailed(Graphics::GetDevice()->CreateShaderResourceView(t->texture_.Get(), &desc, &t->resourceView_));

        return t;
    }

    std::shared_ptr<Texture2D> Texture2D::CreateFromRawTexture(const RawTexture& raw, bool mipmap)
    {
        auto t = std::make_shared<Texture2D>();

        t->width_ = raw.GetWidth();
        t->height_ = raw.GetHeight();
        const auto format = raw.IsHDR() ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
        t->Create(raw.GetRowPitch(), raw.GetData(), format, mipmap);

        return t;
    }

    void Texture2D::Bind(int slot) const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->PSSetShaderResources(slot, 1, resourceView_.GetAddressOf()));
    }

    void Texture2D::BindCS(int slot) const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->CSSetShaderResources(slot, 1, resourceView_.GetAddressOf()));
    }

    void Texture2D::Create(uint32_t rowPitch, void* data, DXGI_FORMAT format, bool mipmap)
    {
        ENGINE_ASSERT(width_ >= 0, "Width can't be negative");
        ENGINE_ASSERT(height_ >= 0, "Height can't be negative");

        auto device = Graphics::GetDevice();

        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = static_cast<UINT>(width_);
            desc.Height = static_cast<UINT>(height_);
            desc.MipLevels = mipmap ? 0 : 1;
            desc.ArraySize = 1;
            desc.Format = format;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = mipmap ? D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = mipmap ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

            DX::AssertIfFailed(device->CreateTexture2D(&desc, NULL, &texture_));
        }

        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->UpdateSubresource(texture_.Get(), 0, NULL, data, rowPitch, 0));

        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MostDetailedMip = 0;
            desc.Texture2D.MipLevels = -1;

            DX::AssertIfFailed(device->CreateShaderResourceView(texture_.Get(), &desc, &resourceView_));            
        }

        if (mipmap)
        {
            WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->GenerateMips(resourceView_.Get()));
        }
    }

}
