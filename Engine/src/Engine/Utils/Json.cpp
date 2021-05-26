#include "pch.h"
#include <fstream>
#include "Json.h"

namespace Engine
{

    std::string Json::ToString(nlohmann::ordered_json json)
    {
        return json.dump(2);
    }

    nlohmann::ordered_json Json::FromString(const std::string& str)
    {
        return nlohmann::json::parse(str);
    }

    nlohmann::ordered_json Json::Load(const std::filesystem::path& filepath)
    {
        std::ifstream i(filepath);
        nlohmann::ordered_json json;
        i >> json;
        return json;
    }

    void Json::Save(nlohmann::ordered_json json, const std::filesystem::path& filepath)
    {
        std::ofstream o(filepath);
        o << ToString(json) << std::endl;
    }

}
