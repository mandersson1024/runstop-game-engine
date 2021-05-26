#pragma once
#include <memory>
#include <filesystem>
#include "Asset.h"

namespace Engine 
{

    class FileTimestamp;
    class ComputeShader;

    class ComputeShaderAsset : public Asset
    {
    public:
        ComputeShaderAsset(const std::filesystem::path& jsonFilepath);

        std::shared_ptr<ComputeShader> GetShader() { return shader_; }

        bool IsDirty() override;
        void LoadSync() override;


    private:
        std::filesystem::path jsonFilepath_;
        std::filesystem::path shaderFilepath_;

        std::unique_ptr<FileTimestamp> jsonTimestamp_;
        std::unique_ptr<FileTimestamp> shaderTimestamp_;

        std::shared_ptr<ComputeShader> shader_;
    };

}

