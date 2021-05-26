#pragma once
#include <memory>
#include "Engine/Application/Layer.h"

namespace Engine
{
    class Event;
    class Mesh;
}

namespace Editor
{

    class SceneLayer : public Engine::Layer
    {
    public:
        SceneLayer();

        void OnAttach() override;
        void OnUpdate(float deltaTime) override;
        void OnEvent(Engine::Event& event) override;

    private:
        std::shared_ptr<Engine::Mesh> grid_;
        std::shared_ptr<Engine::Mesh> frustum_;
    };

}


