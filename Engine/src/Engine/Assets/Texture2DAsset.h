#pragma once
#include <filesystem>
#include "Asset.h"

namespace Engine
{

    class Texture2D;
    class FileTimestamp;

    class Texture2DAsset : public Asset
    {
    public:
        Texture2DAsset(const std::filesystem::path& filepath);

        static void CreateFromTextureFilepathAndSave(const std::filesystem::path& textureFilepath);

        std::shared_ptr<Texture2D> GetTexture() const { return texture_; }

        bool IsDirty() override;
        void LoadSync() override;

    private:
        std::filesystem::path jsonFilepath_;
        std::filesystem::path textureFilepath_;

        std::unique_ptr<FileTimestamp> jsonTimestamp_;
        std::unique_ptr<FileTimestamp> textureTimestamp_;
        
        std::shared_ptr<Texture2D> texture_;
    };

}


