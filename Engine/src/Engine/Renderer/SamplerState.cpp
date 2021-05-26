#include "pch.h"
#include "SamplerState.h"
#include "Graphics.h"

namespace Engine
{

    SamplerState::SamplerState(bool border, bool clamp, DirectX::XMVECTORF32 borderColor, bool point)
    {
        D3D11_SAMPLER_DESC desc
        {
            .Filter = point ? D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            .AddressU = border ? D3D11_TEXTURE_ADDRESS_BORDER : D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressV = border ? D3D11_TEXTURE_ADDRESS_BORDER : D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressW = border ? D3D11_TEXTURE_ADDRESS_BORDER : D3D11_TEXTURE_ADDRESS_WRAP,
            .BorderColor = {borderColor[0], borderColor[1], borderColor[2], borderColor[3] },
            .MaxLOD = D3D11_FLOAT32_MAX
        };

        if (clamp)
        {
            desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        }

        DX::ThrowIfFailed(Graphics::GetDevice()->CreateSamplerState(&desc, &state_));
    }

    void SamplerState::Bind(int slot) const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->PSSetSamplers(slot, 1, state_.GetAddressOf()));
    }

    void SamplerState::BindCS(int slot) const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->CSSetSamplers(slot, 1, state_.GetAddressOf()));
    }

}

