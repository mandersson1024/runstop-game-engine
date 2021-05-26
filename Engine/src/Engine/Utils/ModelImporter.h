#pragma once
#include <memory>
#include <filesystem>

namespace Engine
{

    struct Model;

    class ModelImporter
    {
    public:
        ModelImporter() = delete;

        static std::shared_ptr<Model> Import(const std::filesystem::path& assetFilepath);
    };

}
