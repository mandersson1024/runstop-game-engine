#include "pch.h"
#include "RasterizerState.h"
#include "Graphics.h"

namespace Engine
{

    RasterizerState::RasterizerState(bool showBackface)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = showBackface ? D3D11_CULL_NONE : D3D11_CULL_BACK;
        desc.FrontCounterClockwise = false;
        desc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        desc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = true; // Documentation recommends 'true' always 
        desc.AntialiasedLineEnable = true; // Documentation recommends 'true' always

        DX::ThrowIfFailed(Graphics::GetDevice()->CreateRasterizerState(&desc, &state_));
    }

    void RasterizerState::Bind() const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->RSSetState(state_.Get()));
    }

}

