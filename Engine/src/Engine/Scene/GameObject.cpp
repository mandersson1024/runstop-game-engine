#include "pch.h"
#include "GameObject.h"
#include "GameObjectController.h"
#include "Transform.h"
#include "Engine/Assets/Model.h"
#include "Engine/Renderer/Renderer.h"
#include "../Assets/Assets.h"

namespace Engine
{

    std::vector<std::shared_ptr<Engine::GameObject>> GameObject::GetChildren() const
    {
        //
        // TODO:
        // Consider the performance when using this one 
        // for now, as it creates a new vector.
        //
        // If we change the implementation to returning
        // a range, that should be better. Not sure what
        // the performance of ranges are, though.
        //
        // Passing around shared GameObjects should be
        // changed to passing ids anyway, at some point.
        //

        std::vector<std::shared_ptr<GameObject>> children;

        for (auto& transform : m_transform->GetChildren())
        {
            children.push_back(transform->GetGameObject());
        }

        return children;
    }

    void GameObject::OnUpdateControllers(float deltaTime)
    {
        if (!m_active)
        {
            return;
        }

        if (m_controller)
        {
            m_controller->OnUpdate(deltaTime);
        }

        for (auto& child : m_transform->GetChildren())
        {
            child->GetGameObject()->OnUpdateControllers(deltaTime);
        }
    }

    void GameObject::OnUpdate(float deltaTime)
    {
        if (!m_active)
        {
            return;
        }

        auto modelMatrix = m_transform->GetWorldModelMatrix();

        if (HasModel())
        {
            std::shared_ptr<Model> model = Assets::GetModel(m_modelId);

            for (auto& part : model->parts)
            {
                if (part.active)
                {
                    if (part.materialIndex < materialIds_.size())
                    {
                        const auto materialId = materialIds_[part.materialIndex];
                        Application::GetRenderer().SubmitMesh(part.mesh, materialId, modelMatrix, true, m_debugInvertColors);
                    }
                    else
                    {
                        Application::GetRenderer().SubmitMesh(part.mesh, "error", modelMatrix, true, false);
                    }
                }
            }
        }

        for (auto& child : m_transform->GetChildren())
        {
            child->GetGameObject()->OnUpdate(deltaTime);
        }
    }


}

