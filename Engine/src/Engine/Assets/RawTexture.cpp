#include "pch.h"
#include "RawTexture.h"
#include "..\EngineAssert.h"
#include "stb\stb_image.h"

namespace Engine
{

    RawTexture::RawTexture(const std::filesystem::path& filepath)
    {
        int numColorChannelsInSource;
        //stbi_set_flip_vertically_on_load(1);

        hdr_ = stbi_is_hdr(filepath.generic_string().c_str());
        int numColorChannels;

        if (hdr_)
        {
            numColorChannels = STBI_rgb_alpha;
            data_ = stbi_loadf(filepath.generic_string().c_str(), &width_, &height_, &numColorChannelsInSource, numColorChannels);
            rowPitch_ = 4 * width_ * numColorChannels;
        }
        else
        {
            numColorChannels = STBI_rgb_alpha;
            data_ = stbi_load(filepath.generic_string().c_str(), &width_, &height_, &numColorChannelsInSource, numColorChannels);
            rowPitch_ = width_ * numColorChannels;
        }

        ENGINE_ASSERT(data_, std::format("Failed to load image {} ({})", filepath.generic_string().c_str(), stbi_failure_reason()));
    }

    RawTexture::~RawTexture()
    {
        stbi_image_free(data_);
    }

}
