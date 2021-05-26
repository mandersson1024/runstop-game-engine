#pragma once
#include <wrl/client.h>
#include <d3d11.h>

namespace Engine
{

    class BlendState
    {
    public:
        BlendState(bool transparent);

        void Bind() const;

    private:
        Microsoft::WRL::ComPtr<ID3D11BlendState> blendState_;
    };

}

