#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

namespace Engine
{

    bool AssetFileExists(const std::filesystem::path& assetFilepath);
    
    void AddFilenamesInDirectoryToVector(const std::filesystem::path& dir, std::vector<std::string>& out_filepaths);
    std::vector<std::string> GetFilenamesInDirectory(const std::filesystem::path& dir);

    std::vector<std::filesystem::path> GetRelativeFilepathsRecursively(const std::filesystem::path& root);
}

