#include "pch.h"
#include "BlendState.h"
#include "Graphics.h"

namespace Engine
{

    BlendState::BlendState(bool transparent)
        : blendState_(nullptr)
    {
        D3D11_BLEND_DESC blendDesc{};

        // If a transparent texture closer to the camera is rendered
        // before something farther away, then the farther one will be
        // clipped by the transparent area.
        //
        // Setting AlphaToCoverageEnable to true will fix this.
        // (Sorting the objects is another solution that can work.)

        blendDesc.AlphaToCoverageEnable = transparent;

        blendDesc.RenderTarget[0].BlendEnable = transparent;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        DX::ThrowIfFailed(Graphics::GetDevice()->CreateBlendState(&blendDesc, &blendState_));
    }

    void BlendState::Bind() const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->OMSetBlendState(blendState_.Get(), nullptr, 0xffffffff));
    }

}

