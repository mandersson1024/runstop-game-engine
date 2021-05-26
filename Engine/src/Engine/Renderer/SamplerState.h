#pragma once
#include <wrl/client.h>
#include <d3d11.h>

namespace Engine
{

    class SamplerState
    {
    public:
        SamplerState(bool border, bool clamp, DirectX::XMVECTORF32 borderColor, bool point);

        void Bind(int slot) const;
        void BindCS(int slot) const;

    private:
        Microsoft::WRL::ComPtr<ID3D11SamplerState> state_;
    };

}

