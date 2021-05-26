#pragma once
#include <string>
#include <memory>
#include <unordered_map>

namespace Engine 
{
    class Texture2DAsset;
    class Texture2D;
    class EnvironmentMapAsset;
    class TextureCube;
    class ModelAsset;
    class ShaderAsset;
    class Shader;
    class ComputeShaderAsset;
    class ComputeShader;
    class MaterialAsset;
    class Mesh;
    class SamplerState;
    class BlendState;
    class RasterizerState;
    class SceneAsset;
    class Scene;
    struct Material;
    struct Model;

    class Assets
    {
    public:
        static void Init();
        static void ScanFileSystem();
        static void ReloadDirtyAssets();
        static void AwaitThreads();

        static std::shared_ptr<Texture2DAsset> GetTexture2DAsset(const std::string& id);
        static std::shared_ptr<Texture2D> GetTexture2D(const std::string& id, const std::string& placeholderId);
        static std::shared_ptr<EnvironmentMapAsset> GetEnvironmentMapAsset(const std::string& id);
        static std::shared_ptr<TextureCube> GetSkybox(const std::string& id);
        static std::shared_ptr<TextureCube> GetRadianceMap(const std::string& id);
        static std::shared_ptr<TextureCube> GetIrradianceMap(const std::string& id);
        static std::shared_ptr<ModelAsset> GetModelAsset(const std::string& id);
        static std::shared_ptr<Model> GetModel(const std::string& id);
        static std::shared_ptr<ShaderAsset> GetShaderAsset(const std::string& id);
        static std::shared_ptr<Shader> GetShader(const std::string& id);
        static std::shared_ptr<ComputeShaderAsset> GetComputeShaderAsset(const std::string& id);
        static std::shared_ptr<ComputeShader> GetComputeShader(const std::string& id);
        static std::shared_ptr<MaterialAsset> GetMaterialAsset(const std::string& id);
        static std::shared_ptr<Material> GetMaterial(const std::string& id);
        static std::shared_ptr<Mesh> GetMesh(const std::string& id);
        static std::shared_ptr<SamplerState> GetSamplerState(const std::string& id);
        static std::shared_ptr<BlendState> GetBlendState(const std::string& id);
        static std::shared_ptr<RasterizerState> GetRasterizerState(const std::string& id);
        static std::shared_ptr<SceneAsset> GetSceneAsset(const std::string& id);
        static std::shared_ptr<Scene> GetScene(const std::string& id);
        
        static std::unordered_map<std::string, std::shared_ptr<EnvironmentMapAsset>>& GetAllEnvironmentMapAssets();
        static std::unordered_map<std::string, std::shared_ptr<MaterialAsset>>& GetAllMaterialAssets();
        static std::unordered_map<std::string, std::shared_ptr<SceneAsset>>& GetAllSceneAssets();

        static std::shared_ptr<Texture2D> GetBasecolorMapTexture(const std::string& id) { return GetTexture2D(id, "default_white"); }
        static std::shared_ptr<Texture2D> GetRoughnessMapTexture(const std::string& id) { return GetTexture2D(id, "default_white"); }
        static std::shared_ptr<Texture2D> GetMetallicMapTexture(const std::string& id) { return GetTexture2D(id, "default_black"); }
        static std::shared_ptr<Texture2D> GetAmbientOcclusionMapTexture(const std::string& id) { return GetTexture2D(id, "default_white"); }
        static std::shared_ptr<Texture2D> GetNormalMapTexture(const std::string& id) { return GetTexture2D(id, "default_normal_map"); }
        static std::shared_ptr<Texture2D> GetDefaultTextureWhite() { return GetTexture2D("default_white", ""); }
        static std::shared_ptr<Texture2D> GetDefaultTextureBlack() { return GetTexture2D("default_black", ""); }
        static std::shared_ptr<Texture2D> GetDefaultTextureRed() { return GetTexture2D("default_red", ""); }
        static std::shared_ptr<Texture2D> GetDefaultTextureGreen() { return GetTexture2D("default_green", ""); }
        static std::shared_ptr<Texture2D> GetDefaultTextureBlue() { return GetTexture2D("default_blue", ""); }
        static std::shared_ptr<Texture2D> GetDefaultTextureYellow() { return GetTexture2D("default_yellow", ""); }
        static std::shared_ptr<Texture2D> GetDefaultTextureCyan() { return GetTexture2D("default_cyan", ""); }
        static std::shared_ptr<Texture2D> GetDefaultTextureMagenta() { return GetTexture2D("default_magenta", ""); }
        static std::shared_ptr<Texture2D> GetDefaultNormalMap() { return GetTexture2D("default_normal_map", ""); }
        static std::shared_ptr<Texture2D> GetBRDFTexture() { return GetTexture2D("textures/brdf.png", "default_white"); }
        static std::shared_ptr<Material> GetErrorMaterial() { return GetMaterial("error"); }
        static std::shared_ptr<Material> GetShadowMapMaterial() { return GetMaterial("shadow_map"); }
        static std::shared_ptr<Material> GetSkyboxMaterial() { return GetMaterial("skybox"); }
        static std::shared_ptr<Material> GetSkyboxPointSamplerMaterial() { return GetMaterial("skybox_point_sampler"); }
        static std::shared_ptr<Shader> GetErrorShader() { return GetShader("error"); }

    private:
        static std::unique_ptr<Assets> instance_;

        std::unordered_map<std::string, std::shared_ptr<Texture2D>> builtInTexture2Ds_;
        std::unordered_map<std::string, std::shared_ptr<TextureCube>> builtInTextureCubes_;
        std::unordered_map<std::string, std::shared_ptr<Model>> builtInModels_;
        std::unordered_map<std::string, std::shared_ptr<Shader>> builtInShaders_;
        std::unordered_map<std::string, std::shared_ptr<Material>> builtInMaterials_;
        std::unordered_map<std::string, std::shared_ptr<Mesh>> builtInMeshes_;
        std::unordered_map<std::string, std::shared_ptr<SamplerState>> builtInSamplerStates_;
        std::unordered_map<std::string, std::shared_ptr<BlendState>> builtInBlendStates_;
        std::unordered_map<std::string, std::shared_ptr<RasterizerState>> builtInRasterizerStates_;
        std::unordered_map<std::string, std::shared_ptr<ComputeShaderAsset>> builtInComputeShaderAssets_;

        std::unordered_map<std::string, std::shared_ptr<Texture2DAsset>> texture2DAssets_;
        std::unordered_map<std::string, std::shared_ptr<EnvironmentMapAsset>> environmentMapAssets_;
        std::unordered_map<std::string, std::shared_ptr<ModelAsset>> modelAssets_;
        std::unordered_map<std::string, std::shared_ptr<ShaderAsset>> shaderAssets_;
        std::unordered_map<std::string, std::shared_ptr<MaterialAsset>> materialAssets_;
        std::unordered_map<std::string, std::shared_ptr<SceneAsset>> sceneAssets_;
    };

}
