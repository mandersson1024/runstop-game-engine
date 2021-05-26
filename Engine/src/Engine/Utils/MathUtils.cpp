#include "pch.h"
#include "MathUtils.h"
#include <numbers>

namespace Engine
{

    float DegreesToRadians(float degrees) noexcept
    {
        return degrees * ((float)std::numbers::pi / 180.f);
    }

    float RadiansToDegrees(float radians) noexcept
    {
        return radians / ((float)std::numbers::pi / 180.f);
    }

    float GammaToLinear(float value) noexcept
    {
        // This is just an approximation
        return static_cast<float>(pow(value, 2.2f));
    }

    float LinearToGamma(float value) noexcept
    {
        // This is just an approximation
        return static_cast<float>(pow(value, 0.4545454545f));
    }

    DirectX::XMVECTOR GammaToLinear(DirectX::XMVECTOR color) noexcept
    {
        return
        {
            GammaToLinear(DirectX::XMVectorGetX(color)),
            GammaToLinear(DirectX::XMVectorGetY(color)),
            GammaToLinear(DirectX::XMVectorGetZ(color)),
            DirectX::XMVectorGetW(color)
        };
    }

    DirectX::XMVECTOR LinearToGamma(DirectX::XMVECTOR color) noexcept
    {
        return
        {
            LinearToGamma(DirectX::XMVectorGetX(color)),
            LinearToGamma(DirectX::XMVectorGetY(color)),
            LinearToGamma(DirectX::XMVectorGetZ(color)),
            DirectX::XMVectorGetW(color)
        };
    }

}