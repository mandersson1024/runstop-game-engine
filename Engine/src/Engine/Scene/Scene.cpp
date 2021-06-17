#include "pch.h"
#include "Scene.h"
#include "Transform.h"
#include "GameObject.h"
#include "GameObjectController.h"
#include "..\Renderer\LightSource.h"
#include "..\Application\Application.h"
#include "..\Renderer\Renderer.h"

namespace Engine
{

    std::shared_ptr<Camera> Scene::GetCamera(const std::string& name)
    {
        for (auto transform : transforms_)
        {
            if (const auto camera = transform->FindCamera(name))
            {
                return camera;
            }
        }

        return nullptr;
    }

    void Scene::AddAllCamerasToVector(std::vector<std::shared_ptr<Camera>>& cameras)
    {
        lightSource_->GetTransform()->AddAllCamerasToVector(cameras);
                 
        for (auto transform : transforms_)
        {
            transform->AddAllCamerasToVector(cameras);
        }
    }

    void Scene::OnEvent(Event& evt)
    {
        for (auto transform : transforms_)
        {
            if (transform->HasGameObject())
            {
                auto gameObject = transform->GetGameObject();

                if (gameObject->HasController())
                {
                    auto controller = gameObject->GetController();

                    if (evt.GetEventType() == EventType::MouseDown)
                    {
                        controller->OnMouseDown(static_cast<MouseDownEvent&>(evt));
                    }

                    if (evt.GetEventType() == EventType::MouseUp)
                    {
                        controller->OnMouseUp(static_cast<MouseUpEvent&>(evt));
                    }

                    if (evt.GetEventType() == EventType::MouseMove)
                    {
                        controller->OnMouseMove(static_cast<MouseMoveEvent&>(evt));
                    }
                }
            }
        }
    }

    void Scene::OnUpdateControllers(float deltaTime)
    {
        for (auto transform : transforms_)
        {
            if (transform->HasGameObject())
            {
                transform->GetGameObject()->OnUpdateControllers(deltaTime);
            }
        }
    }

    void Scene::OnUpdate(float deltaTime)
    {
        Application::GetRenderer().SetLightSource(lightSource_);

        for (auto transform : transforms_)
        {
            if (transform->HasGameObject())
            {
                transform->GetGameObject()->OnUpdate(deltaTime);
            }
        }
    }

}

