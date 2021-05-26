#pragma once
#include <memory>
#include <vector>
#include <string>

namespace Engine
{
    class LightSource;
    class Transform;
    class Camera;

    class Scene
    {
    public:
        void OnEvent(Event& evt);
        void OnUpdateControllers(float deltaTime);
        void OnUpdate(float deltaTime);

        std::shared_ptr<LightSource> GetLightSource() const { return lightSource_; }
        void SetLightSource(std::shared_ptr<LightSource> light) { lightSource_ = light; }

        const std::vector<std::shared_ptr<Transform>>& GetTransforms() const { return transforms_; }
        void AddTransform(std::shared_ptr<Transform> child) { transforms_.push_back(child); }

        std::shared_ptr<Camera> GetCamera(const std::string& name);
        void AddAllCamerasToVector(std::vector<std::shared_ptr<Camera>>& cameras);

        void SetEnvironmentMapId(const std::string& id) { environmentMapId_ = id; }
        bool HasEnvironmentMap() const { return !environmentMapId_.empty(); }
        std::string GetEnvironmentMapId() const { return environmentMapId_; }

    private:
        std::shared_ptr<LightSource> lightSource_;
        std::vector<std::shared_ptr<Transform>> transforms_;
        std::string environmentMapId_ = "";
    };

}
