#pragma once
#include <string>
#include <DirectXMath.h>

namespace Engine
{

    struct Material
    {
        std::string shaderId = "shaders/pbr/pbr";
        std::string blendStateId = "default";
        std::string rasterizerStateId = "default";
        std::string samplerStateId = "default";

        DirectX::XMFLOAT3A baseColorTint = { 1, 1, 1 };
        std::string basecolorMapId = "default_white";

        DirectX::XMFLOAT2A roughnessRange = { 0, 1 };
        DirectX::XMFLOAT2A metallicRange = { 0, 1 };
        DirectX::XMFLOAT2A ambientOcclusionRange = { 0, 1 };

        std::string roughnessMapId = "default_black";
        std::string metallicMapId = "default_black";
        std::string ambientOcclusionMapId = "default_white";
        std::string normalMapId = "default_normal_map";

        float uvRepeat = 1;
    };

}
