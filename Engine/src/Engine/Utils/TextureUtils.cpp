#include "pch.h"
#include "TextureUtils.h"
#include "..\Renderer\Texture2D.h"
#include "..\Renderer\TextureCube.h"
#include "..\Assets\Assets.h"

namespace Engine
{

    std::shared_ptr<Texture2D> CreateSinglePixelTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        uint8_t data[4]{ r, g, b, a };
        return Texture2D::CreateFromPixelData(1, 1, 4, data, false);
    }

    std::shared_ptr<TextureCube> CreateSinglePixelTextureCube(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        auto texture2D = CreateSinglePixelTexture(r, g, b, a);
        auto tex = texture2D->GetTexture();
        return std::make_shared<TextureCube>(tex, tex, tex, tex, tex, tex);
    }

    std::shared_ptr<TextureCube> CreateMultiColorTextureCube()
    {
        auto cube = std::make_shared<TextureCube>(
            Assets::GetDefaultTextureRed()->GetTexture(),
            Assets::GetDefaultTextureGreen()->GetTexture(),
            Assets::GetDefaultTextureBlue()->GetTexture(),
            Assets::GetDefaultTextureYellow()->GetTexture(),
            Assets::GetDefaultTextureCyan()->GetTexture(),
            Assets::GetDefaultTextureMagenta()->GetTexture());

        return cube;
    }

}

