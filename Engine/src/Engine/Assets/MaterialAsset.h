#pragma once
#include <memory>
#include <filesystem>
#include "Asset.h"

namespace Engine
{

    class FileTimestamp;
    struct Material;

    class MaterialAsset : public Asset
    {
    public:
        MaterialAsset(const std::filesystem::path& filepath);

        std::shared_ptr<Material> GetMaterial() const { return material_; }

        bool IsDirty() override;
        void LoadSync() override;
        void Save();

    private:
        std::filesystem::path filepath_;
        std::unique_ptr<FileTimestamp> timestamp_;
        std::shared_ptr<Material> material_;
    };

}

