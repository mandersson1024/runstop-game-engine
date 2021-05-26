#include "pch.h"
#include "ProjectSettings.h"
#include "Engine/Assets/AssetPaths.h"
#include "nlohmann/json.hpp"
#include "Engine/EngineAssert.h"

namespace Engine
{

    ProjectSettings::ProjectSettings(const std::filesystem::path& filepath)
        : filepath_(GetAssetsFolderPath() / filepath)
    {
    }

    void ProjectSettings::Load()
    {
        ENGINE_LOG("Loading project settings");
        json_ = Json::Load(filepath_);
    }

    void ProjectSettings::Save()
    {
        ENGINE_LOG("Saving project settings");
        ENGINE_LOG(json_.dump(4));
        Json::Save(json_, filepath_);
    }

    bool ProjectSettings::GetBool(const std::string& name, bool defaultValue)
    {
        if (json_[name] == nullptr)
        {
            SetBool(name, defaultValue);
        }

        return json_[name];
    }

    void ProjectSettings::SetBool(const std::string& name, bool value)
    {
        json_[name] = value;
        Save();
    }

    std::string ProjectSettings::GetString(const std::string& name, std::string defaultValue)
    {
        if (json_[name] == nullptr)
        {
            SetString(name, defaultValue);
        }

        return json_[name];
    }

    void ProjectSettings::SetString(const std::string& name, std::string value)
    {
        json_[name] = value;
        Save();
    }

    float ProjectSettings::GetFloat(const std::string& name, float defaultValue)
    {
        if (json_[name] == nullptr)
        {
            SetFloat(name, defaultValue);
        }

        return json_[name];
    }

    void ProjectSettings::SetFloat(const std::string& name, float value)
    {
        json_[name] = value;
        Save();
    }

    int ProjectSettings::GetInt(const std::string& name, int defaultValue)
    {
        if (json_[name] == nullptr)
        {
            SetInt(name, defaultValue);
        }

        return json_[name];
    }

    void ProjectSettings::SetInt(const std::string& name, int value)
    {
        json_[name] = value;
        Save();
    }

}

