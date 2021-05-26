#include "pch.h"
#include "ComputeShaderAsset.h"
#include "..\Platform\FileTimestamp.h"
#include "..\Renderer\Shader.h"

namespace Engine
{

    ComputeShaderAsset::ComputeShaderAsset(const std::filesystem::path& filepath)
        : jsonFilepath_(filepath)
        , jsonTimestamp_(std::make_unique<FileTimestamp>(GetAssetsFolderPath() / jsonFilepath_))
    {
    }

    void ComputeShaderAsset::LoadSync()
    {
        TimeLog timer(fmt::format("Loading '{}'", jsonFilepath_.generic_string()));

        nlohmann::ordered_json json = Json::Load(GetAssetsFolderPath() / jsonFilepath_);
        jsonTimestamp_->Stamp();

        shaderFilepath_ = json["source"].get<std::string>();
        shaderTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / shaderFilepath_);
        shader_ = std::make_shared<ComputeShader>(GetAssetsFolderPath() / shaderFilepath_);
    }

    bool ComputeShaderAsset::IsDirty()
    {
        return jsonTimestamp_->HasChanged()
            || (shaderTimestamp_ && shaderTimestamp_->HasChanged());
    }


}
