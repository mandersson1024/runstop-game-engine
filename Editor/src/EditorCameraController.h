#pragma once
#include <memory>

namespace Engine
{
    class Camera;
}

namespace Editor
{

    class EditorCameraController
    {
    public:
        EditorCameraController();
        ~EditorCameraController();

        void Reset();

        std::shared_ptr<Engine::Camera> GetCamera() const { return m_camera; }
        void OnUpdate(float deltaTime);
        void OnMouseScroll(float delta);

    private:
        struct MousePosition
        {
            int x, y;
        };

        std::shared_ptr<Engine::Camera> m_camera;
        bool m_wasMousePositionRegisteredLastFrame = false;
        MousePosition m_mousePositionLastFrame{};
        float m_scrollDelta = 0;
    };

}

