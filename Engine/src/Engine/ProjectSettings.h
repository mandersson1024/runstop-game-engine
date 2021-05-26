#pragma once
#include <string>
#include <filesystem>
#include "nlohmann/json.hpp"

namespace Engine
{

    class ProjectSettings
    {
    public:
        ProjectSettings(const std::filesystem::path& filepath);

        void Load();
        void Save();

        std::string GetStartupScene() { return GetString(c_propertyStartupScene, "scenes/empty.scene.json"); }
        void SetStartupScene(const std::string& scene) { SetString(c_propertyStartupScene, scene); }

        bool GetDrawHelperLines() { return GetBool(c_propertyDrawHelperLines, false); }
        void SetDrawHelperLines(bool b) { SetBool(c_propertyDrawHelperLines, b); }

        bool GetDrawColliders() { return GetBool(c_propertyDrawColliders, false); }
        void SetDrawColliders(bool b) { SetBool(c_propertyDrawColliders, b); }

        bool GetRunInEditor() { return GetBool(c_propertyRunInEditor, false); }
        void SetRunInEditor(bool b) { SetBool(c_propertyRunInEditor, b); }

        int GetSkyboxDisplayType() { return GetInt(c_propertySkyboxDisplayType, 0); }
        void SetSkyboxDisplayType(int b) { SetInt(c_propertySkyboxDisplayType, b); }

        float GetRadianceMapRoughness() { return GetFloat(c_propertyRadianceMapRoughness, 0.f); }
        void SetRadianceMapRoughness(float value) { SetFloat(c_propertyRadianceMapRoughness, value); }

        bool GetDrawSkyboxUsingPointSampler() { return GetBool(c_drawSkyboxUsingPointSampler, false); }
        void SetDrawSkyboxUsingPointSampler(bool b) { SetBool(c_drawSkyboxUsingPointSampler, b); }

        bool GetIgnoreLightSources() { return GetBool(c_ignoreLightSources, false); }
        void SetIgnoreLightSources(bool b) { SetBool(c_ignoreLightSources, b); }

        bool GetIgnoreDiffuseIBL() { return GetBool(c_ignoreDiffuseIBL, false); }
        void SetIgnoreDiffuseIBL(bool b) { SetBool(c_ignoreDiffuseIBL, b); }

        bool GetIgnoreSpecularIBL() { return GetBool(c_ignoreSpecularIBL, false); }
        void SetIgnoreSpecularIBL(bool b) { SetBool(c_ignoreSpecularIBL, b); }

        bool GetIgnoreAmbientOcclusion() { return GetBool(c_ignoreAmbientOcclusion, false); }
        void SetIgnoreAmbientOcclusion(bool b) { SetBool(c_ignoreAmbientOcclusion, b); }

    private:

        bool GetBool(const std::string& name, bool defaultValue);
        void SetBool(const std::string& name, bool value);

        std::string GetString(const std::string& name, std::string defaultValue);
        void SetString(const std::string& name, std::string value);

        float GetFloat(const std::string& name, float defaultValue);
        void SetFloat(const std::string& name, float value);

        int GetInt(const std::string& name, int defaultValue);
        void SetInt(const std::string& name, int value);

    private:
        std::filesystem::path filepath_;
        nlohmann::ordered_json json_;

    private:
        const std::string c_propertyStartupScene = "startup_scene";
        const std::string c_propertyDrawHelperLines = "draw_helper_lines";
        const std::string c_propertyDrawColliders = "draw_colliders";
        const std::string c_propertyRunInEditor = "run_in_editor";
        const std::string c_propertySkyboxDisplayType = "skybox_display_type";
        const std::string c_propertyRadianceMapRoughness = "radiance_map_roughness";
        const std::string c_drawSkyboxUsingPointSampler = "draw_skybox_using_point_sampler";
        const std::string c_ignoreLightSources = "ignore_light_sources";
        const std::string c_ignoreDiffuseIBL = "ignore_diffuse_ibl";
        const std::string c_ignoreSpecularIBL = "ignore_specular_ibl";
        const std::string c_ignoreAmbientOcclusion = "ignore_ambient_occlusion";
    };

}
