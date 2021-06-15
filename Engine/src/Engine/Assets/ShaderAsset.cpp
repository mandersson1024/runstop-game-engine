#include "pch.h"
#include "ShaderAsset.h"
#include "..\Renderer\Shader.h"
#include "..\Platform\FileTimestamp.h"

namespace Engine
{

    ShaderAsset::ShaderAsset(const std::filesystem::path& jsonFilepath)
        : jsonFilepath_(jsonFilepath)
        , jsonTimestamp_(std::make_unique<FileTimestamp>(GetAssetsFolderPath() / jsonFilepath_))
    {
    }

    void ShaderAsset::LoadSync()
    {
        TimeLog timer(std::format("Loading '{}'", jsonFilepath_.generic_string()));

        nlohmann::ordered_json json = Json::Load(GetAssetsFolderPath() / jsonFilepath_);
        jsonTimestamp_->Stamp();

        vertexShaderFilepath_ = json["vertex_source"].get<std::string>();
        pixelShaderFilepath_ = json["pixel_source"].get<std::string>();
        vertexType_ = VertexTypeFromString(json["vertex_type"].get<std::string>());

        vertexShaderTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / vertexShaderFilepath_);
        pixelShaderTimestamp_ = std::make_unique<FileTimestamp>(GetAssetsFolderPath() / pixelShaderFilepath_);

        shader_ = std::make_shared<Shader>(GetAssetsFolderPath() / vertexShaderFilepath_, GetAssetsFolderPath() / pixelShaderFilepath_, vertexType_);
    }

    bool ShaderAsset::IsDirty()
    {
        return jsonTimestamp_->HasChanged()
            || (vertexShaderTimestamp_ && vertexShaderTimestamp_->HasChanged())
            || (pixelShaderTimestamp_ && pixelShaderTimestamp_->HasChanged());
    }


}
