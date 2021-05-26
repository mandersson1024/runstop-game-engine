#include "pch.h"
#include "FileTimestamp.h"
#include "Engine/EngineAssert.h"

namespace Engine
{

    FileTimestamp::FileTimestamp(const std::filesystem::path& path)
        : path_(path)
        , stamp_{}
    {
        ENGINE_ASSERT(std::filesystem::exists(path_), "");
        Stamp();
    }

    void FileTimestamp::Stamp()
    {
        ENGINE_ASSERT(std::filesystem::exists(path_), "");
        stamp_ = std::filesystem::last_write_time(path_);
    }

    bool FileTimestamp::HasChanged() const
    {
        ENGINE_ASSERT(std::filesystem::exists(path_), "");

        auto time = std::filesystem::last_write_time(path_);
        if (time > stamp_)
        {
            return true;
        }

        return false;
    }

}
