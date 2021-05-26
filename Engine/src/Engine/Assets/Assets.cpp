#include "pch.h"
#include "Assets.h"
#include "Model.h"
#include "ShaderAsset.h"
#include "ComputeShaderAsset.h"
#include "EnvironmentMapAsset.h"
#include "Texture2DAsset.h"
#include "ModelAsset.h"
#include "MaterialAsset.h"
#include "SceneAsset.h"
#include "..\Renderer\Material.h"
#include "..\Renderer\Shader.h"
#include "..\Utils\MeshUtils.h"
#include "..\Renderer\SamplerState.h"
#include "..\Renderer\BlendState.h"
#include "..\Renderer\RasterizerState.h"
#include "..\Utils\FileUtils.h"
#include "..\Platform\FileTimestamp.h"

namespace Engine
{

    std::unique_ptr<Assets> Assets::instance_;

    void Assets::Init()
    {
        instance_ = std::make_unique<Assets>();

        ENGINE_LOG_INFO("Initializing assets");

        instance_->builtInTexture2Ds_["default_white"] = CreateSinglePixelTexture(255, 255, 255, 255);
        instance_->builtInTexture2Ds_["default_black"] = CreateSinglePixelTexture(0, 0, 0, 255);
        instance_->builtInTexture2Ds_["default_red"] = CreateSinglePixelTexture(255, 0, 0, 255);
        instance_->builtInTexture2Ds_["default_green"] = CreateSinglePixelTexture(0, 255, 0, 255);
        instance_->builtInTexture2Ds_["default_blue"] = CreateSinglePixelTexture(0, 0, 255, 255);
        instance_->builtInTexture2Ds_["default_yellow"] = CreateSinglePixelTexture(255, 255, 0, 255);
        instance_->builtInTexture2Ds_["default_cyan"] = CreateSinglePixelTexture(0, 255, 255, 255);
        instance_->builtInTexture2Ds_["default_magenta"] = CreateSinglePixelTexture(255, 0, 255, 255);
        instance_->builtInTexture2Ds_["default_normal_map"] = CreateSinglePixelTexture(128, 128, 255, 255);

        instance_->builtInTextureCubes_["default_black"] = CreateSinglePixelTextureCube(0, 0, 0, 255);

        instance_->builtInModels_["default_empty"] = std::make_shared<Model>();

        instance_->builtInMaterials_["default"] = std::make_shared<Material>();

        instance_->builtInMaterials_["error"] = std::make_shared<Material>();
        instance_->builtInMaterials_["error"]->shaderId = "shaders/error/error";
        instance_->builtInMaterials_["shadow_map"] = std::make_shared<Material>();
        instance_->builtInMaterials_["shadow_map"]->shaderId = "shaders/shadow_map/shadow_map";
        instance_->builtInMaterials_["shadow_map"]->samplerStateId = "shadow_map";
        instance_->builtInMaterials_["skybox"] = std::make_shared<Material>();
        instance_->builtInMaterials_["skybox"]->shaderId = "shaders/skybox/skybox";
        instance_->builtInMaterials_["skybox"]->samplerStateId = "skybox";
        instance_->builtInMaterials_["skybox_point_sampler"] = std::make_shared<Material>();
        instance_->builtInMaterials_["skybox_point_sampler"]->shaderId = "shaders/skybox/skybox";
        instance_->builtInMaterials_["skybox_point_sampler"]->samplerStateId = "skybox_point";

        instance_->builtInMeshes_["quad"] = CreateQuadMesh();
        instance_->builtInMeshes_["skybox"] = CreateSkyboxMesh();

        instance_->builtInSamplerStates_["default"] = std::make_shared<SamplerState>(false, false, DirectX::Colors::Black, false);
        instance_->builtInSamplerStates_["point"] = std::make_shared<SamplerState>(false, false, DirectX::Colors::Red, true);
        instance_->builtInSamplerStates_["shadow_map"] = std::make_shared<SamplerState>(true, false, DirectX::Colors::Red, false);
        instance_->builtInSamplerStates_["skybox"] = std::make_shared<SamplerState>(false, false, DirectX::Colors::Black, false);
        instance_->builtInSamplerStates_["skybox_point"] = std::make_shared<SamplerState>(false, false, DirectX::Colors::Red, true);
        instance_->builtInSamplerStates_["compute"] = std::make_shared<SamplerState>(false, true, DirectX::Colors::Magenta, false);
        instance_->builtInSamplerStates_["brdf"] = std::make_shared<SamplerState>(false, true, DirectX::Colors::White, false);

        instance_->builtInBlendStates_["default"] = std::make_shared<BlendState>(false);
        instance_->builtInBlendStates_["transparent"] = std::make_shared<BlendState>(true);

        instance_->builtInRasterizerStates_["default"] = std::make_shared<RasterizerState>(false);

        {
            //
            // We want the error shader to be available at all times and have no errors.
            //

            auto asset = ShaderAsset("shaders/error/error.shader.json");
            asset.LoadSync();
            auto shader = asset.GetShader();
            ENGINE_ASSERT(shader->IsSuccessfullyCompiled(), "");
            instance_->builtInShaders_["error"] = shader;
        }

        {
            //
            // We want the loading shader to be available at all times and have no errors.
            //

            auto asset = ShaderAsset("shaders/loading/loading.shader.json");
            asset.LoadSync();
            auto shader = asset.GetShader();
            ENGINE_ASSERT(shader->IsSuccessfullyCompiled(), "");
            instance_->builtInShaders_["loading"] = shader;
        }

        {
            //
            // We don't want to load the compute shaders concurrently, because we have
            // no fallbacks for them. So we register them by hand and load them here.
            // 

            instance_->builtInComputeShaderAssets_["equirectangular_to_cube"] = std::make_shared<ComputeShaderAsset>("shaders/compute/equirectangular_to_cube.compute_shader.json");
        }

        ScanFileSystem();
    }

    void Assets::ScanFileSystem()
    {
        TimeLog timer("Scanning filesystem for assets");

        auto paths = GetRelativeFilepathsRecursively(GetAssetsFolderPath());

        for (const std::filesystem::path& path : paths)
        {
            const auto& pathString = path.generic_string();
            const auto& extension = path.extension();

            if (pathString.ends_with(".environment.json"))
            {
                //
                // REGISTER ENVIRONMENT ASSET
                //

                auto id = RemoveSuffix(pathString, ".environment.json");
                auto asset = std::make_shared<EnvironmentMapAsset>(path);
                instance_->environmentMapAssets_[id] = asset;
            }
            else if (extension == ".png" || extension == ".hdr" || extension == ".jpg" || extension == ".jpeg" || extension == ".gif")
            {
                //
                // CREATE AND REGISTER MISSING TEXTURE ASSET FILE
                //

                std::filesystem::path assetPath = path;
                assetPath += ".texture.json";

                bool assetFileExists = std::filesystem::exists(GetAssetsFolderPath() / assetPath);
                if (!assetFileExists)
                {
                    Texture2DAsset::CreateFromTextureFilepathAndSave(path);
                    auto asset = std::make_shared<Texture2DAsset>(assetPath);
                    instance_->texture2DAssets_[path.generic_string()] = asset;
                }
            }
            else if (pathString.ends_with(".texture.json"))
            {
                //
                // REGISTER TEXTURE ASSET
                //

                auto id = RemoveSuffix(pathString, ".texture.json");
                auto asset = std::make_shared<Texture2DAsset>(path);
                instance_->texture2DAssets_[id] = asset;
            }
            else if (extension == ".obj" || extension == ".fbx")
            {
                //
                // CREATE AND REGISTER MISSING MOODEL ASSET FILE
                //

                std::filesystem::path assetPath = path;
                assetPath += ".model.json";

                bool assetFileExists = std::filesystem::exists(GetAssetsFolderPath() / assetPath);
                if (!assetFileExists)
                {
                    ModelAsset::CreateFromModelFilepathAndSave(path);
                    auto asset = std::make_shared<ModelAsset>(assetPath);
                    instance_->modelAssets_[path.generic_string()] = asset;
                }
            }
            else if (pathString.ends_with(".model.json"))
            {
                //
                // REGISTER MODEL ASSET
                //

                auto id = RemoveSuffix(pathString, ".model.json");
                auto asset = std::make_shared<ModelAsset>(path);
                instance_->modelAssets_[id] = asset;
            }
            else if (pathString.ends_with(".shader.json"))
            {
                //
                // REGISTER SHADER ASSET
                //

                auto id = RemoveSuffix(pathString, ".shader.json");
                auto asset = std::make_shared<ShaderAsset>(path);
                instance_->shaderAssets_[id] = asset;
            }
            else if (pathString.ends_with(".material.json"))
            {
                //
                // REGISTER MATERIAL ASSET
                //

                auto id = RemoveSuffix(pathString, ".material.json");
                auto asset = std::make_shared<MaterialAsset>(path);
                instance_->materialAssets_[id] = asset;
            }
            else if (pathString.ends_with(".scene.json"))
            {
                //
                // REGISTER SCENE ASSET
                //

                auto id = RemoveSuffix(pathString, ".scene.json");
                auto asset = std::make_shared<SceneAsset>(path);
                instance_->sceneAssets_[id] = asset;
            }
        }
    }

    void Assets::ReloadDirtyAssets()
    {
        for (auto& it : instance_->texture2DAssets_)
        { 
            it.second->ReloadIfDirty(); 
        }

        for (auto& it : instance_->environmentMapAssets_)
        {
            it.second->ReloadIfDirty();
        }

        for (auto& it : instance_->modelAssets_)
        {
            it.second->ReloadIfDirty();
        }

        for (auto& it : instance_->shaderAssets_)
        {
            it.second->ReloadIfDirty();
        }

        for (auto& it : instance_->materialAssets_)
        {
            it.second->ReloadIfDirty();
        }
    }

    void Assets::AwaitThreads()
    {
        for (auto& it : instance_->texture2DAssets_)
        {
            it.second->Await();
        }

        for (auto& it : instance_->environmentMapAssets_)
        {
            it.second->Await();
        }

        for (auto& it : instance_->modelAssets_)
        {
            it.second->Await();
        }

        for (auto& it : instance_->shaderAssets_)
        {
            it.second->Await();
        }

        for (auto& it : instance_->builtInComputeShaderAssets_)
        {
            it.second->Await();
        }

        for (auto& it : instance_->materialAssets_)
        {
            it.second->Await();
        }
    }

    std::shared_ptr<Texture2DAsset> Assets::GetTexture2DAsset(const std::string& id)
    {
        ENGINE_ASSERT(instance_->texture2DAssets_.contains(id), "Missing environment map");
        return instance_->texture2DAssets_[id];
    }

    std::shared_ptr<Texture2D> Assets::GetTexture2D(const std::string& id, const std::string& placeholderId)
    {
        if (instance_->builtInTexture2Ds_.contains(id))
        {
            return instance_->builtInTexture2Ds_[id];
        }

        auto asset = GetTexture2DAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            asset->Load();
        }

        if (asset->GetLoadingState() == AssetLoadingState::Loaded)
        {
            return asset->GetTexture();
        }
        
        return instance_->builtInTexture2Ds_[placeholderId];
    }

    std::shared_ptr<EnvironmentMapAsset> Assets::GetEnvironmentMapAsset(const std::string& id)
    {
        ENGINE_ASSERT(instance_->environmentMapAssets_.contains(id), "Missing environment map asset");
        return instance_->environmentMapAssets_[id];
    }

    std::shared_ptr<TextureCube> Assets::GetSkybox(const std::string& id)
    {
        auto asset = GetEnvironmentMapAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            asset->Load();
        }

        if (asset->GetLoadingState() == AssetLoadingState::Loaded)
        {
            return asset->GetSkybox();
        }
        
        return instance_->builtInTextureCubes_["default_black"];
    }

    std::shared_ptr<TextureCube> Assets::GetRadianceMap(const std::string& id)
    {
        auto asset = GetEnvironmentMapAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            asset->Load();
        }

        if (asset->GetLoadingState() == AssetLoadingState::Loaded)
        {
            return asset->GetRadianceMap();
        }
        
        return instance_->builtInTextureCubes_["default_black"];
    }

    std::shared_ptr<TextureCube> Assets::GetIrradianceMap(const std::string& id)
    {
        auto asset = GetEnvironmentMapAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            asset->Load();
        }

        if (asset->GetLoadingState() == AssetLoadingState::Loaded)
        {
            return asset->GetIrradianceMap();
        }
        
        return instance_->builtInTextureCubes_["default_black"];
    }

    std::shared_ptr<ModelAsset> Assets::GetModelAsset(const std::string& id)
    {
        ENGINE_ASSERT(instance_->modelAssets_.contains(id), "Missing model asset");
        return instance_->modelAssets_[id];
    }

    std::shared_ptr<Model> Assets::GetModel(const std::string& id)
    {
        if (instance_->builtInModels_.contains(id))
        {
            return instance_->builtInModels_[id];
        }

        auto asset = GetModelAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            asset->Load();
        }

        if (asset->GetLoadingState() == AssetLoadingState::Loaded)
        {
            return asset->GetModel();
        }
        
        return instance_->builtInModels_["default_empty"];
    }

    std::shared_ptr<ShaderAsset> Assets::GetShaderAsset(const std::string& id)
    {
        ENGINE_ASSERT(instance_->shaderAssets_.contains(id), "Missing shader asset");
        return instance_->shaderAssets_[id];
    }

    std::shared_ptr<ComputeShaderAsset> Assets::GetComputeShaderAsset(const std::string& id)
    {
        ENGINE_ASSERT(instance_->builtInComputeShaderAssets_.contains(id), "Missing compute shader asset");
        return instance_->builtInComputeShaderAssets_[id];
    }

    std::shared_ptr<Shader> Assets::GetShader(const std::string& id)
    {
        if (instance_->builtInShaders_.contains(id))
        {
            return instance_->builtInShaders_[id];
        }

        auto asset = GetShaderAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            asset->Load();
        }

        if (asset->GetLoadingState() == AssetLoadingState::Loaded)
        {
            auto shader = asset->GetShader();

            if (shader->IsSuccessfullyCompiled())
            {
                return shader;
            }
            else
            {
                return instance_->builtInShaders_["error"];
            }
        }
        
        return instance_->builtInShaders_["loading"];
    }

    std::shared_ptr<ComputeShader> Assets::GetComputeShader(const std::string& id)
    {
        auto asset = GetComputeShaderAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            //
            // We have no fallback for compute shaders, so we have to wait until it is loaded.
            //

            asset->LoadSync();

            ENGINE_ASSERT(asset->GetShader()->IsSuccessfullyCompiled(), "Error compiling compute shader");
        }

        return asset->GetShader();
    }

    std::unordered_map<std::string, std::shared_ptr<EnvironmentMapAsset>>& Assets::GetAllEnvironmentMapAssets()
    {
        return instance_->environmentMapAssets_;
    }

    std::shared_ptr<MaterialAsset> Assets::GetMaterialAsset(const std::string& id)
    {
        ENGINE_ASSERT(instance_->materialAssets_.contains(id), "Missing material asset");
        return instance_->materialAssets_[id];
    }

    std::shared_ptr<Material> Assets::GetMaterial(const std::string& id)
    {
        if (instance_->builtInMaterials_.contains(id))
        {
            return instance_->builtInMaterials_[id];
        }

        auto asset = GetMaterialAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            asset->Load();
        }

        if (asset->GetLoadingState() == AssetLoadingState::Loaded)
        {
            return asset->GetMaterial();
        }

        return instance_->builtInMaterials_["default"];
    }

    std::unordered_map<std::string, std::shared_ptr<MaterialAsset>>& Assets::GetAllMaterialAssets()
    {
        return instance_->materialAssets_;
    }

    std::shared_ptr<Mesh> Assets::GetMesh(const std::string& id)
    {
        ENGINE_ASSERT(instance_->builtInMeshes_.contains(id), "Missing mesh");
        return instance_->builtInMeshes_[id];
    }

    std::shared_ptr<SamplerState> Assets::GetSamplerState(const std::string& id)
    {
        ENGINE_ASSERT(instance_->builtInSamplerStates_.contains(id), "Missing sampler state");
        return instance_->builtInSamplerStates_[id];
    }

    std::shared_ptr<BlendState> Assets::GetBlendState(const std::string& id)
    {
        ENGINE_ASSERT(instance_->builtInBlendStates_.contains(id), "Missing blend state");
        return instance_->builtInBlendStates_[id];
    }

    std::shared_ptr<RasterizerState> Assets::GetRasterizerState(const std::string& id)
    {
        ENGINE_ASSERT(instance_->builtInRasterizerStates_.contains(id), "Missing rasterizer state");
        return instance_->builtInRasterizerStates_[id];
    }

    std::shared_ptr<SceneAsset> Assets::GetSceneAsset(const std::string& id)
    {
        ENGINE_ASSERT(instance_->sceneAssets_.contains(id), "Missing scene asset");
        return instance_->sceneAssets_[id];
    }

    std::shared_ptr<Engine::Scene> Assets::GetScene(const std::string& id)
    {
        auto asset = GetSceneAsset(id);

        if (asset->GetLoadingState() == AssetLoadingState::NotLoaded)
        {
            // We don't bother loading scenes in the background for now.
            // At this point, the puropse of multi-threaded asset loading is speed,
            // and async scene loading would not help us much with this. It will 
            // only add complexity.
            // 
            // At some later point we might want
            // to load subscenes, or to load a scene in the background during gameplay.
            // But that's for later.
            
            asset->LoadSync();
            asset->SetLoadingState(AssetLoadingState::Loaded);

            ENGINE_ASSERT(asset->GetScene(), "We expect the scene to be loaded");
        }

        return instance_->sceneAssets_[id]->GetScene();

    }

    std::unordered_map<std::string, std::shared_ptr<SceneAsset>>& Assets::GetAllSceneAssets()
    {
        return instance_->sceneAssets_;
    }

}
