#pragma once
#include <memory>
#include <filesystem>
#include <vector>

namespace Engine
{

    class Mesh;

    struct ModelPart
    {
        bool active = true;
        std::string name;
        std::shared_ptr<Mesh> mesh;
        uint32_t materialIndex;
        std::string materialImportName;
    };

    struct Model
    {
        std::filesystem::path assetFilepath;
        std::vector<ModelPart> parts;
    };

}

