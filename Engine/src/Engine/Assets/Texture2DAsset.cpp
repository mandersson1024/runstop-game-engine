#include "pch.h"
#include "Texture2DAsset.h"
#include "nlohmann\json.hpp"
#include "..\Platform\FileTimestamp.h"
#include "RawTexture.h"
#include "..\Renderer\Texture2D.h"

namespace Engine 
{

    Texture2DAsset::Texture2DAsset(const std::filesystem::path& filepath) 
        : jsonFilepath_(filepath)
        , jsonTimestamp_(std::make_unique<FileTimestamp>(GetAssetsFolderPath() / jsonFilepath_))
    {
    }

    void Texture2DAsset::CreateFromTextureFilepathAndSave(const std::filesystem::path& textureFilepath)
    {
        std::filesystem::path assetFilepath = textureFilepath;
        assetFilepath += ".texture.json";

        ENGINE_LOG("Creating new Texture2DAsset '{}'", assetFilepath.generic_string());

        nlohmann::ordered_json json
        {
            { "texture_filepath", textureFilepath.generic_string() },
            { "sampler_state",    "default"                        },
            { "mipmap",           true                             },
        };

        Json::Save(json, GetAssetsFolderPath() / assetFilepath);
    }

    void Texture2DAsset::LoadSync()
    {
        TimeLog timer(fmt::format("Loading '{}'", jsonFilepath_.generic_string()));

        nlohmann::ordered_json json = Json::Load(GetAssetsFolderPath() / jsonFilepath_);
        jsonTimestamp_->Stamp();

        textureFilepath_ = json["texture_filepath"].get<std::string>();
        textureTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / textureFilepath_);
        auto mipmap = json["mipmap"].get<bool>();

        auto raw = RawTexture(GetAssetsFolderPath() / textureFilepath_);
        texture_ = Texture2D::CreateFromRawTexture(raw, mipmap);
    }

    bool Texture2DAsset::IsDirty()
    {
        return jsonTimestamp_->HasChanged() 
            || (textureTimestamp_ && textureTimestamp_->HasChanged());
    }

}

