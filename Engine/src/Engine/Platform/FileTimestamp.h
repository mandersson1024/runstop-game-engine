#pragma once
#include <filesystem>

namespace Engine
{

    class FileTimestamp
    {
    public:
        FileTimestamp(const std::filesystem::path& path);
    
        void Stamp();
        bool HasChanged() const;

    private:
        std::filesystem::path path_;
        std::filesystem::file_time_type stamp_;
    };

}

