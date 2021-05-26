#pragma once
#include <filesystem>
#include "Asset.h"

namespace Engine 
{

    class FileTimestamp;
    struct Model;

    class ModelAsset : public Asset
    {
    public:
        ModelAsset(const std::filesystem::path& filepath);

        static void CreateFromModelFilepathAndSave(const std::filesystem::path& modelFilepath);
        std::shared_ptr<Model> GetModel() const { return model_; }

        bool IsDirty() override;
        void LoadSync() override;

    private:
        std::filesystem::path jsonFilepath_;
        std::filesystem::path modelFilepath_;
        
        std::unique_ptr<FileTimestamp> jsonTimestamp_;
        std::unique_ptr<FileTimestamp> modelTimestamp_;
        
        std::shared_ptr<Model> model_;
    };

}
