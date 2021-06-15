#include "pch.h"
#include "MaterialAsset.h"
#include "..\Platform\FileTimestamp.h"
#include "..\Renderer\Material.h"

namespace Engine
{

    MaterialAsset::MaterialAsset(const std::filesystem::path& filepath)
        : filepath_(filepath)
        , timestamp_(std::make_unique<FileTimestamp>(GetAssetsFolderPath() / filepath_))
    {
    }

    void MaterialAsset::LoadSync()
    {
        TimeLog timer(std::format("Loading '{}'", filepath_.generic_string()));

        auto json = Json::Load(GetAssetsFolderPath() / filepath_);
        timestamp_->Stamp();

        material_ = std::make_shared<Material>();

        material_->shaderId              = json["shader"].get<std::string>();
        material_->blendStateId          = json["blend_state"].get<std::string>();
        material_->rasterizerStateId     = json["rasterizer_state"].get<std::string>();
        material_->samplerStateId        = json["sampler_state"].get<std::string>();
        material_->basecolorMapId        = json["basecolor_map"].get<std::string>();
        material_->roughnessMapId        = json["roughness_map"].get<std::string>();
        material_->metallicMapId         = json["metallic_map"].get<std::string>();
        material_->ambientOcclusionMapId = json["ambient_occlusion_map"].get<std::string>();
        material_->normalMapId           = json["normal_map"].get<std::string>();
        material_->uvRepeat              = json["uv_repeat"];

        material_->baseColorTint =
        {
            json["basecolor_tint"][0],
            json["basecolor_tint"][1],
            json["basecolor_tint"][2]
        };

        material_->roughnessRange =
        {
            json["roughness_range"][0],
            json["roughness_range"][1]
        };

        material_->metallicRange =
        {
            json["metallic_range"][0],
            json["metallic_range"][1]
        };

        material_->ambientOcclusionRange =
        {
            json["ambient_occlusion_range"][0],
            json["ambient_occlusion_range"][1]
        };
    }

    void MaterialAsset::Save()
    {
        TimeLog timer(std::format("Saving material '{}'", filepath_.generic_string()));

        std::array<float, 4> basecolorTint
        {
            {
                material_->baseColorTint.x,
                material_->baseColorTint.y,
                material_->baseColorTint.z
            }
        };

        std::array<float, 2> roughnessRange
        {
            {
                material_->roughnessRange.x,
                material_->roughnessRange.y
            }
        };

        std::array<float, 2> metallicRange
        {
            {
                material_->metallicRange.x,
                material_->metallicRange.y
            }
        };

        std::array<float, 2> ambientOcclusionRange
        {
            {
                material_->ambientOcclusionRange.x,
                material_->ambientOcclusionRange.y
            }
        };

        nlohmann::ordered_json json
        {
            { "shader",                  material_->shaderId              },
            { "blend_state",             material_->blendStateId          },
            { "rasterizer_state",        material_->rasterizerStateId     },
            { "sampler_state",           material_->samplerStateId        },
            { "basecolor_tint",          basecolorTint                    },
            { "basecolor_map",           material_->basecolorMapId        },
            { "roughness_map",           material_->roughnessMapId        },
            { "metallic_map",            material_->metallicMapId         },
            { "ambient_occlusion_map",   material_->ambientOcclusionMapId },
            { "normal_map",              material_->normalMapId           },
            { "uv_repeat",               material_->uvRepeat              },
            { "roughness_range",         roughnessRange                   },
            { "metallic_range",          metallicRange                    },
            { "ambient_occlusion_range", ambientOcclusionRange            },
        };

        //ENGINE_LOG(json.dump(4));
        Json::Save(json, GetAssetsFolderPath() / filepath_);
    }

    bool MaterialAsset::IsDirty()
    {
        return timestamp_->HasChanged();
    }

}
