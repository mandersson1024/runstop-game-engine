#include "pch.h"
#include "EnvironmentMapAsset.h"
#include "..\Platform\FileTimestamp.h"
#include "..\Utils\ComputeShaderTextureFilter.h"
#include "RawTexture.h"
#include "..\Renderer\Texture2D.h"
#include "..\Renderer\TextureCube.h"

namespace Engine 
{

    EnvironmentMapAsset::EnvironmentMapAsset(const std::filesystem::path& filepath)
        : jsonFilepath_(filepath)
        , jsonTimestamp_(std::make_unique<FileTimestamp>(GetAssetsFolderPath() / jsonFilepath_))
    {
    }

    void EnvironmentMapAsset::LoadSync()
    {
        {
            //
            // Load .environment.json file
            //

            TimeLog timer(std::format("Loading '{}'", jsonFilepath_.generic_string()));

            const auto path = (GetAssetsFolderPath() / jsonFilepath_).generic_string();
            const auto json = Json::Load(path);
            jsonTimestamp_->Stamp();

            skyboxFilepath_ = json["skybox"].get<std::string>();
            radianceMapFilepath_ = json["radiance_map"].get<std::string>();
            irradianceMapFilepath_ = json["irradiance_map"].get<std::string>();

            skyboxTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / skyboxFilepath_);
            radianceMapTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / radianceMapFilepath_);
            irradianceMapTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / irradianceMapFilepath_);
        }

        {
            //
            // Load and process skybox
            //

            TimeLog timer(std::format("Loading '{}'", skyboxFilepath_.generic_string()));
                
            const auto raw = RawTexture(GetAssetsFolderPath() / skyboxFilepath_);
            const auto source = Texture2D::CreateFromRawTexture(raw, false);
            const auto filter = ComputeShaderTextureFilter();
            skybox_ = filter.EquirectangularToCube(*source, 1024);
        }

        {
            //
            // Load and process irradiance map
            //

            TimeLog timer(std::format("Loading '{}'", irradianceMapFilepath_.generic_string()));
                
            const auto raw = RawTexture(GetAssetsFolderPath() / irradianceMapFilepath_);
            const auto source = Texture2D::CreateFromRawTexture(raw, false);
            const auto filter = ComputeShaderTextureFilter();
            irradianceMap_ = filter.EquirectangularToCube(*source, 256);
        }

        {
            //
            // Load radiance map
            //

            TimeLog timer(std::format("Loading '{}'", radianceMapFilepath_.generic_string()));
                
            radianceMap_ = std::make_shared<TextureCube>(GetAssetsFolderPath() / radianceMapFilepath_);
        }
    }

    bool EnvironmentMapAsset::IsDirty()
    {
        return jsonTimestamp_->HasChanged()
            || (skyboxTimestamp_ && skyboxTimestamp_->HasChanged())
            || (radianceMapTimestamp_ && radianceMapTimestamp_->HasChanged())
            || (irradianceMapTimestamp_ && irradianceMapTimestamp_->HasChanged());
    }

}

