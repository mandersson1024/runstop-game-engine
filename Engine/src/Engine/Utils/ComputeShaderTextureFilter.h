#pragma once

namespace Engine
{

    class Texture2D;

    class ComputeShaderTextureFilter
    {
    public:
        std::shared_ptr<TextureCube> EquirectangularToCube(const Texture2D& source, uint16_t resolution) const;

    };

}

