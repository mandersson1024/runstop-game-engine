#include "pch.h"
#include "ModelAsset.h"
#include "..\Platform\FileTimestamp.h"
#include "..\Utils\ModelImporter.h"

namespace Engine
{

    ModelAsset::ModelAsset(const std::filesystem::path& filepath)
        : jsonFilepath_(filepath)
        , jsonTimestamp_(std::make_unique<FileTimestamp>(GetAssetsFolderPath() / jsonFilepath_))
    {
    }

    void ModelAsset::CreateFromModelFilepathAndSave(const std::filesystem::path& modelFilepath)
    {
        std::filesystem::path assetFilepath = modelFilepath;
        assetFilepath += ".model.json";

        ENGINE_LOG("Creating new ModelAsset '{}'", assetFilepath.generic_string());

        nlohmann::ordered_json json
        {
            { "model_filepath", modelFilepath.generic_string() },
        };

        Json::Save(json, GetAssetsFolderPath() / assetFilepath);
    }

    void ModelAsset::LoadSync()
    {
        TimeLog timer(std::format("Loading '{}'", jsonFilepath_.generic_string()));

        const auto json = Json::Load(GetAssetsFolderPath() / jsonFilepath_);
        jsonTimestamp_->Stamp();

        modelFilepath_ = json["model_filepath"].get<std::string>();
        modelTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / modelFilepath_);

        model_ = ModelImporter::Import(modelFilepath_);
    }

    bool ModelAsset::IsDirty()
    {
        return jsonTimestamp_->HasChanged() 
            || (modelTimestamp_ && modelTimestamp_->HasChanged());
    }
}

