#pragma once
#include <memory>

namespace Engine
{

    class Texture2D;
    class TextureCube;

    std::shared_ptr<Texture2D> CreateSinglePixelTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    std::shared_ptr<TextureCube> CreateSinglePixelTextureCube(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    std::shared_ptr<TextureCube> CreateMultiColorTextureCube();

}

