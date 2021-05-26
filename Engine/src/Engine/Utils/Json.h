#pragma once
#include <filesystem>
#include "nlohmann/json.hpp"

namespace Engine
{
    class Json
    {
    public:
        Json() = delete;

        static std::string ToString(nlohmann::ordered_json json);
        static nlohmann::ordered_json FromString(const std::string& str);

        static nlohmann::ordered_json Load(const std::filesystem::path& filepath);
        static void Save(nlohmann::ordered_json json, const std::filesystem::path& filepath);
    };

}
