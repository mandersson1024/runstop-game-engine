#include "pch.h"
#include "StringUtils.h"
#include <stringapiset.h>

namespace Engine
{

    std::wstring StringToWString(const std::string& str)
    {
        if (str.empty())
        {
            return std::wstring();
        }

        int num_chars = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, str.c_str(), static_cast<int>(str.length()), NULL, 0);

        std::wstring wstrTo;

        if (num_chars)
        {
            wstrTo.resize(num_chars);
            if (MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, str.c_str(), static_cast<int>(str.length()), &wstrTo[0], num_chars))
            {
                return wstrTo;
            }
        }

        return std::wstring();
    }

    std::vector<std::string> SplitString(std::string str, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
            token = str.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(str.substr(pos_start));
        return res;
    }

    std::string RemovePrefix(const std::string& path, const std::string& prefix)
    {
        std::string result(path);
        result.erase(0, prefix.length());
        return result;
    }

    std::string RemoveSuffix(const std::string& path, const std::string& suffix)
    {
        std::string result(path);
        result.erase(result.find(suffix));
        return result;
    }

}

