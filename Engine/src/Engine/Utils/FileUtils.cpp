#include "pch.h"
#include "FileUtils.h"
#include <filesystem>
#include "Engine/Assets/AssetPaths.h"

namespace Engine
{

    bool AssetFileExists(const std::filesystem::path& assetFilepath)
    {
        return std::filesystem::exists(GetAssetsFolderPath() / assetFilepath);
    }

    void AddFilenamesInDirectoryToVector(const std::filesystem::path& dir, std::vector<std::string>& out_filepaths)
    {
        auto it = std::filesystem::directory_iterator(dir);

        for (auto& entry : it)
        {
            //if (entry.path().has_filename()) // should always be true
            {
                std::string name(entry.path().filename().generic_string());
                out_filepaths.push_back(name);
            }
        }
    }

    std::vector<std::string> GetFilenamesInDirectory(const std::filesystem::path& dir)
    {
        std::vector<std::string> paths;
        AddFilenamesInDirectoryToVector(dir, paths);
        return paths;
    }

    std::vector<std::filesystem::path> GetRelativeFilepathsRecursively(const std::filesystem::path& root)
    {
        std::vector<std::filesystem::path> vector;
        auto it = std::filesystem::recursive_directory_iterator(root);

        for (const auto& path : it)
        {
            if (path.is_regular_file())
            {
                auto relative = std::filesystem::relative(path, root);
                vector.push_back(relative);
            }
        }

        return vector;
    }

}

