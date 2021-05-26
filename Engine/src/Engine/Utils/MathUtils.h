#pragma once
#include <DirectXMath.h>

namespace Engine
{
    float DegreesToRadians(float degrees) noexcept;
    float RadiansToDegrees(float radians) noexcept;

    float GammaToLinear(float value) noexcept;
    float LinearToGamma(float value) noexcept;

    DirectX::XMVECTOR GammaToLinear(DirectX::XMVECTOR color) noexcept;
    DirectX::XMVECTOR LinearToGamma(DirectX::XMVECTOR color) noexcept;

}

