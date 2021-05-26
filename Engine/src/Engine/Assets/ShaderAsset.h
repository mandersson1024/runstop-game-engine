#pragma once
#include <memory>
#include <filesystem>
#include "Asset.h"
#include "..\Renderer\VertexTypes.h"

namespace Engine
{

    class Shader;
    class FileTimestamp;

    class ShaderAsset : public Asset
    {
    public:
        ShaderAsset(const std::filesystem::path& jsonFilepath);

        bool IsDirty() override;
        void LoadSync() override;

        std::shared_ptr<Shader> GetShader() { return shader_; }

    private:
        std::filesystem::path jsonFilepath_;
        std::filesystem::path vertexShaderFilepath_;
        std::filesystem::path pixelShaderFilepath_;

        std::unique_ptr<FileTimestamp> jsonTimestamp_;
        std::unique_ptr<FileTimestamp> vertexShaderTimestamp_;
        std::unique_ptr<FileTimestamp> pixelShaderTimestamp_;

        VertexType vertexType_;

        std::shared_ptr<Shader> shader_;
    };

}

