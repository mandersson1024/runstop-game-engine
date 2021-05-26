#pragma once
#include <wrl/client.h>
#include <d3d11.h>

namespace Engine
{

    class RasterizerState
    {
    public:
        RasterizerState(bool showBackface);
        void Bind() const;

    private:
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> state_;
    };

}

