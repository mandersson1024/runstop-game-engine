#pragma once
#include <filesystem>
#include "Asset.h"
#include "nlohmann\json.hpp"

namespace Engine
{

    class Scene;
    class Transform;
    class BoxCollider;
    class GameObject;
    class Camera;
    class LightSource;

    class SceneAsset : public Asset
    {
    public:
        SceneAsset(const std::filesystem::path& filepath);
        
        bool IsDirty() override;
        void LoadSync() override;
        void Save();

        std::shared_ptr<Scene> GetScene() const { return scene_; }

    private:
        std::shared_ptr<Scene> Deserialize(nlohmann::ordered_json json);
        std::shared_ptr<Transform> DeserializeTransform(nlohmann::ordered_json json);

        nlohmann::ordered_json Serialize();
        nlohmann::ordered_json SerializeTransforms(const std::vector<std::shared_ptr<Transform>>& transforms);
        nlohmann::ordered_json SerializeTransform(std::shared_ptr<Transform> transform);
        nlohmann::ordered_json SerializeCollider(std::shared_ptr<BoxCollider> collider);
        nlohmann::ordered_json SerializeGameObject(std::shared_ptr<GameObject> gameObject);
        nlohmann::ordered_json SerializeCamera(std::shared_ptr<Camera> camera);
        nlohmann::ordered_json SerializeLightSource(std::shared_ptr<LightSource> light);
        nlohmann::ordered_json SerializeMaterials(std::vector<std::string> materialIds);

    private:
        std::filesystem::path filepath_;
        std::shared_ptr<Scene> scene_;
    };

}

