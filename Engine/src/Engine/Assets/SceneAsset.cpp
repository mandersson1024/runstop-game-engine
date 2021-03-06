#include "pch.h"
#include "SceneAsset.h"
#include "..\Logger.h"
#include "..\Scene\Scene.h"
#include "..\Scene\Transform.h"
#include "..\Renderer\LightSource.h"
#include "..\Scene\Camera.h"
#include "..\Scene\GameObject.h"
#include "..\Scene\GameObjectController.h"
#include "..\Raycast\Raycast.h"

namespace Engine
{
    SceneAsset::SceneAsset(const std::filesystem::path& filepath)
        : filepath_(filepath)
    {
    }

    void SceneAsset::LoadSync()
    {
        ENGINE_LOG("Loading scene '{}'", filepath_.generic_string());
        TimeLog timer(std::format("Loaded scene {}", filepath_.generic_string()));

        const auto& path = GetAssetsFolderPath() / filepath_;
        ENGINE_ASSERT(std::filesystem::exists(path), std::format("File '{}' not found", path.generic_string()));
        const auto& json = Json::Load(path);
        scene_ = Deserialize(json);
    }

    bool SceneAsset::IsDirty()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void SceneAsset::Save()
    {
        ENGINE_LOG("Saving scene '{}'", filepath_.generic_string());
        auto json = Serialize();

        //ENGINE_LOG(json.dump(2));
        Json::Save(json, GetAssetsFolderPath() / filepath_);
    }

    std::shared_ptr<Scene> SceneAsset::Deserialize(nlohmann::ordered_json json)
    {
        std::shared_ptr<Scene> scene = std::make_shared<Scene>();

        {
            if (json.contains("environment_map"))
            { 
                scene->SetEnvironmentMapId(json["environment_map"]);
            }

            std::shared_ptr<Transform> transform;

            if (json.contains("directional_light"))
            {
                transform = DeserializeTransform(json["directional_light"]);
            }
            else
            {
                transform = std::make_shared<Transform>();
                transform->SetName("Directional Light");
            }

            if (!transform->HasLightSource())
            {
                auto light = std::make_shared<LightSource>();
                light->SetTransform(transform);
                transform->SetLightSource(light);
            }

            if (!transform->HasCamera())
            {
                auto camera = std::make_shared<Camera>();
                camera->SetTransform(transform);
                transform->SetCamera(camera);
            }

            scene->SetLightSource(transform->GetLightSource());
        }

        nlohmann::ordered_json transforms = json["transforms"];
        for (auto& transformJson : transforms)
        {
            std::shared_ptr<Transform> transform = DeserializeTransform(transformJson);
            scene->AddTransform(transform);
        }

        return scene;
    }

    std::shared_ptr<Engine::Transform> SceneAsset::DeserializeTransform(nlohmann::ordered_json json)
    {
        auto transform = std::make_shared<Transform>();

        if (json.contains("name"))
        {
            transform->SetName(json["name"]);
        }
        else
        {
            transform->SetName("");
        }

        if (json.contains("position"))
        {
            transform->SetPosition(
                json["position"][0],
                json["position"][1],
                json["position"][2]
            );
        }

        if (json.contains("scale"))
        {
            transform->SetScale(
                json["scale"][0],
                json["scale"][1],
                json["scale"][2]
            );
        }

        if (json.contains("orientation"))
        {
            EulerAngles eulerAngles
            {
                .x = DegreesToRadians(json["orientation"][0]),
                .y = DegreesToRadians(json["orientation"][1]),
                .z = DegreesToRadians(json["orientation"][2]),
            };

            transform->SetEulerAngles(eulerAngles);
        }

        {
            if (json.contains("collider"))
            {
                std::shared_ptr<BoxCollider> collider = std::make_shared<BoxCollider>();

                collider->SetSize(
                    json["collider"]["size"][0], 
                    json["collider"]["size"][1], 
                    json["collider"]["size"][2]);
                
                collider->SetOffset(
                    json["collider"]["offset"][0], 
                    json["collider"]["offset"][1], 
                    json["collider"]["offset"][2]);

                collider->SetTransform(transform);
                transform->SetCollider(collider);
            }
        }

        if (json.contains("game_object"))
        {
            const auto& gameObjectJson = json["game_object"];
            std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
            transform->SetGameObject(go);
            go->SetTransform(transform);

            if (gameObjectJson.contains("model"))
            {
                go->SetModelId(gameObjectJson["model"].get<std::string>());
            }

            if (gameObjectJson.contains("materials"))
            {
                nlohmann::ordered_json materials = gameObjectJson["materials"];

                for (nlohmann::ordered_json& materialJson : materials)
                {
                    go->AddMaterialId(materialJson.get<std::string>());
                }
            }
            else
            {
                go->AddMaterialId("pbr");
            }

            if (gameObjectJson.contains("controller"))
            {
                auto controller = GameObjectController::GetLibrary().Get(gameObjectJson["controller"])();
                controller->SetGameObject(go);
                go->SetController(controller);
            }
        }

        if (json.contains("camera"))
        {
            const auto& cameraJson = json["camera"];
            std::shared_ptr<Camera> camera = std::make_shared<Camera>();

            if (cameraJson.contains("type"))
            {
                const auto type = cameraJson["type"] == "perspective" ? CameraType::Perspective : CameraType::Orthographic;
                camera->SetType(type);
            }

            if (cameraJson.contains("fov"))
            {
                camera->SetFieldOfView(DegreesToRadians(cameraJson["fov"]));
            }

            if (cameraJson.contains("near_clipping_plane"))
            {
                camera->SetNearClippingPlane(cameraJson["near_clipping_plane"]);
            }

            if (cameraJson.contains("far_clipping_plane"))
            {
                camera->SetFarClippingPlane(cameraJson["far_clipping_plane"]);
            }

            if (cameraJson.contains("ortho_view_width"))
            {
                camera->SetOrthoViewWidth(cameraJson["ortho_view_width"]);
            }

            if (cameraJson.contains("ortho_view_height"))
            {
                camera->SetOrthoViewHeight(cameraJson["ortho_view_height"]);
            }

            camera->SetTransform(transform);
            transform->SetCamera(camera);
        }

        if (json.contains("light_source"))
        {
            std::shared_ptr<LightSource> light = std::make_shared<LightSource>();

            if (json["light_source"].contains("color"))
            {
                light->SetColor(
                    json["light_source"]["color"][0],
                    json["light_source"]["color"][1],
                    json["light_source"]["color"][2]
                );
            }

            transform->SetLightSource(light);
            light->SetTransform(transform);
        }

        if (json.contains("children"))
        {
            for (const auto& childJson : json["children"])
            {
                auto child = DeserializeTransform(childJson);
                Transform::AttachChildToParent(transform, child, false);
            }
        }

        return transform;
    }

    nlohmann::ordered_json SceneAsset::Serialize()
    {
        nlohmann::ordered_json json
        {
            { "environment_map", scene_->GetEnvironmentMapId() },
            { "directional_light", SerializeTransform(scene_->GetLightSource()->GetTransform()) },
            { "transforms", SerializeTransforms(scene_->GetTransforms()) }
        };

        return json;
    }

    nlohmann::ordered_json SceneAsset::SerializeTransforms(const std::vector<std::shared_ptr<Transform>>& transforms)
    {
        std::vector<nlohmann::ordered_json> jsons;

        for (auto& transform : transforms)
        {
            jsons.push_back(SerializeTransform(transform));
        }

        return jsons;
    }

    nlohmann::ordered_json SceneAsset::SerializeTransform(std::shared_ptr<Transform> transform)
    {
        const auto& position = transform->GetPosition();
        const auto eulerAngles = transform->GetEulerAngles();
        const auto& scale = transform->GetScale();

        nlohmann::ordered_json json;

        if (!transform->GetName().empty())
        {
            json["name"] = transform->GetName();
        }

        if (DirectX::XMVectorGetX(position) != 0 ||
            DirectX::XMVectorGetY(position) != 0 ||
            DirectX::XMVectorGetZ(position) != 0)
        {
            json["position"] = { DirectX::XMVectorGetX(position), DirectX::XMVectorGetY(position), DirectX::XMVectorGetZ(position) };
        }

        if (RadiansToDegrees(eulerAngles.x) != 0 ||
            RadiansToDegrees(eulerAngles.y) != 0 ||
            RadiansToDegrees(eulerAngles.z) != 0)
        {
            json["orientation"] =
            {
                RadiansToDegrees(eulerAngles.x),
                RadiansToDegrees(eulerAngles.y),
                RadiansToDegrees(eulerAngles.z)
            };
        }

        if (DirectX::XMVectorGetX(scale) != 1 ||
            DirectX::XMVectorGetY(scale) != 1 ||
            DirectX::XMVectorGetZ(scale) != 1)
        {
            json["scale"] = { DirectX::XMVectorGetX(scale), DirectX::XMVectorGetY(scale), DirectX::XMVectorGetZ(scale) };
        }

        if (transform->HasCollider())
        {
            json["collider"] = SerializeCollider(transform->GetCollider());
        }

        if (transform->HasGameObject())
        {
            json["game_object"] = SerializeGameObject(transform->GetGameObject());
        }

        if (transform->HasCamera())
        {
            json["camera"] = SerializeCamera(transform->GetCamera());
        }

        if (transform->HasLightSource())
        {
            json["light_source"] = SerializeLightSource(transform->GetLightSource());
        }

        if (transform->HasChildren())
        {
            json["children"] = SerializeTransforms(transform->GetChildren());
        }

        return json;
    }

    nlohmann::ordered_json SceneAsset::SerializeCollider(std::shared_ptr<BoxCollider> collider)
    {
        nlohmann::ordered_json json;

        json["size"] = 
        { 
            collider->GetSizeX(), 
            collider->GetSizeY(),
            collider->GetSizeZ(),
        };

        json["offset"] =
        {
            collider->GetOffsetX(),
            collider->GetOffsetY(),
            collider->GetOffsetZ(),
        };

        return json;
    }

    nlohmann::ordered_json SceneAsset::SerializeGameObject(std::shared_ptr<GameObject> gameObject)
    {
        nlohmann::ordered_json json;

        if (gameObject->HasModel())
        {
            json["model"] = gameObject->GetModelId();
        }
        
        json["materials"] = SerializeMaterials(gameObject->GetMaterialIds());

        if (gameObject->HasController())
        {
            json["controller"] = gameObject->GetController()->GetName();
        }

        return json;
    }

    nlohmann::ordered_json SceneAsset::SerializeCamera(std::shared_ptr<Camera> camera)
    {
        nlohmann::ordered_json json;

        json["type"] = camera->GetType() == CameraType::Perspective ? "perspective" : "orthographic";
        json["fov"] = RadiansToDegrees(camera->GetFieldOfView());
        json["near_clipping_plane"] = camera->GetNearClippingPlane();
        json["far_clipping_plane"] = camera->GetFarClippingPlane();
        json["ortho_view_width"] = camera->GetOrthoViewWidth();
        json["ortho_view_height"] = camera->GetOrthoViewHeight();
        
        return json;
    }

    nlohmann::ordered_json SceneAsset::SerializeLightSource(std::shared_ptr<LightSource> light)
    {
        nlohmann::ordered_json json;

        json["color"] =
        {
            light->GetColor().x,
            light->GetColor().y,
            light->GetColor().z
        };

        return json;
    }

    nlohmann::ordered_json SceneAsset::SerializeMaterials(std::vector<std::string> materialIds)
    {
        std::vector<nlohmann::ordered_json> json;

        for (const auto& id : materialIds)
        {
            json.push_back(id);
        }

        return json;
    }

}
