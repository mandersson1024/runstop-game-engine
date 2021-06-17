#pragma once
#include <string>
#include <vector>


namespace Engine
{
    class Transform;
    class GameObjectController;
    class Mesh;
    class Shader;
    class Texture2D;
    class SamplerState;
    class BlendState;
    class RasterizerState;


    class GameObject
    {
    public:
        std::shared_ptr<Transform> GetTransform() const { return transform_; }
        //const std::string& GetName() const { return m_name; }
        std::string GetModelId() const { return modelId_; }
        const std::vector<std::string>& GetMaterialIds() const { return materialIds_; }
        std::vector<std::shared_ptr<GameObject>> GetChildren() const;

        bool IsActive() const { return active_; }
        bool HasModel() const { return !modelId_.empty(); }

        void SetTransform(std::shared_ptr<Transform> transform) { transform_ = transform; }
        //void SetName(const std::string& name) { m_name = name; }
        void SetModelId(const std::string& modelId) { modelId_ = modelId; }
        void AddMaterialId(const std::string& id) { materialIds_.push_back(id); }
        void SetActive(bool active) { active_ = active; }

        bool HasController() const { return controller_ != nullptr; }
        void SetController(std::shared_ptr<GameObjectController> controller) { controller_ = controller; }
        std::shared_ptr<GameObjectController> GetController() const { return controller_; }

        void SetDebugInvertColors(bool invert) { debugInvertColors_ = invert; }
        bool GetDebugInvertColors() const { return debugInvertColors_; }
        void SetDebugInvertColorsRecursively(bool invert)
        { 
            debugInvertColors_ = invert; 

            for (auto child : GetChildren())
            {
                child->SetDebugInvertColorsRecursively(invert);
            }
        }
        

        void OnUpdateControllers(float deltaTime);
        void OnUpdate(float deltaTime);

    private:
        std::shared_ptr<Transform> transform_;
        std::string modelId_;
        std::vector<std::string> materialIds_;
        std::shared_ptr<GameObjectController> controller_;
        bool active_ = true;
        bool debugInvertColors_ = false;
    };

}
