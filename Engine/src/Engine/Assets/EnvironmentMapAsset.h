#pragma once
#include <filesystem>
#include <memory>
#include "Asset.h"

namespace Engine
{

    class TextureCube;
    class FileTimestamp;

    class EnvironmentMapAsset : public Asset
    {
    public:
        EnvironmentMapAsset(const std::filesystem::path& filepath);

        std::shared_ptr<TextureCube> GetSkybox() const { return skybox_; }
        std::shared_ptr<TextureCube> GetRadianceMap() const { return radianceMap_; }
        std::shared_ptr<TextureCube> GetIrradianceMap() const { return irradianceMap_; }

        bool IsDirty() override;
        void LoadSync() override;

    private:
        std::filesystem::path jsonFilepath_;
        std::filesystem::path skyboxFilepath_;
        std::filesystem::path radianceMapFilepath_;
        std::filesystem::path irradianceMapFilepath_;

        std::unique_ptr<FileTimestamp> jsonTimestamp_;
        std::unique_ptr<FileTimestamp> skyboxTimestamp_;
        std::unique_ptr<FileTimestamp> radianceMapTimestamp_;
        std::unique_ptr<FileTimestamp> irradianceMapTimestamp_;

        std::shared_ptr<TextureCube> skybox_;
        std::shared_ptr<TextureCube> irradianceMap_;
        std::shared_ptr<TextureCube> radianceMap_;
    };

}


