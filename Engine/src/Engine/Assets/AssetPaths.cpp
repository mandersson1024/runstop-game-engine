#include "pch.h"
#include "AssetPaths.h"
#include <filesystem>
#include <debugapi.h>

namespace Engine
{

    const std::filesystem::path& GetAssetsFolderPath()
    {
        std::filesystem::path p(IsDebuggerPresent() ? "../Engine/assets" : "assets");
        p.make_preferred();
        static const std::filesystem::path path(std::filesystem::absolute(p));
        return path;
    }

}

