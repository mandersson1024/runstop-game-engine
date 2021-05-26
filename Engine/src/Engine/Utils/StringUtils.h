#pragma once
#include <string>
#include <vector>

namespace Engine
{

    std::wstring StringToWString(const std::string& str);

    std::vector<std::string> SplitString(std::string str, std::string delimiter);

    std::string RemovePrefix(const std::string& path, const std::string& prefix);
    std::string RemoveSuffix(const std::string& path, const std::string& suffix);

}

