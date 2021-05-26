#pragma once
#include <memory>
#include <optional>

namespace Engine
{
    class Transform;
    class Mesh;
    class Camera;
    class GameObject;
    struct ModelPart;
}

namespace Editor
{ 

    class SceneGui
    {
    public:
        void Draw();

    private:
        void DrawDirectionalLight(std::shared_ptr<Engine::Transform> transform);
        void DrawCollider(std::shared_ptr<Engine::Transform> transform);
        void DrawCamera(std::shared_ptr<Engine::Transform> transform, int imGuiWidgetId);
        void DrawGameObject(std::shared_ptr<Engine::Transform> transform, int imGuiWidgetId);
        bool DrawTransform(std::shared_ptr<Engine::Transform> transform, int imGuiWidgetId);
        bool DrawPosition(std::shared_ptr<Engine::Transform> transform);
        bool DrawOrientation(std::shared_ptr<Engine::Transform> transform);
        bool DrawScale(std::shared_ptr<Engine::Transform> transform);
        void DrawModelPart(std::shared_ptr<Engine::GameObject> gameObject, Engine::ModelPart& part, int imGuiWidgetId = 0);
        void DrawMesh(std::shared_ptr<Engine::Mesh> mesh);

    private:
        std::optional<std::shared_ptr<Engine::Camera>> m_selectedCamera;
    };

}


