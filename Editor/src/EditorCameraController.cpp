#include "pch.h"
#include "EditorCameraController.h"
#include "Engine/Platform/Input.h"
#include "Engine\Scene\Camera.h"
#include "Engine/Scene/Transform.h"

namespace Editor
{

    EditorCameraController::EditorCameraController()
        : m_camera(std::make_shared<Engine::Camera>())
    {
        Reset();
    }

    EditorCameraController::~EditorCameraController()
    {
    }

    void EditorCameraController::Reset()
    {
        m_camera->SetType(Engine::CameraType::Perspective);
        m_camera->SetNearClippingPlane(0.1f);
        m_camera->SetFarClippingPlane(1000);
        m_camera->SetFieldOfView(Engine::DegreesToRadians(60));
    }

    void EditorCameraController::OnUpdate(float deltaTime)
    {
        if (Engine::Input::IsRightMouseButtonDown() || m_scrollDelta != 0)
        {
            float moveSpeed = 10.f;
            float rotationSpeed = 2.f;

            if (Engine::Input::IsAnyShiftKeyDown())
            {
                moveSpeed /= 8;
            }

            if (Engine::Input::IsAnyCtrlKeyDown())
            {
                moveSpeed *= 5;
            }

            MousePosition mouseDelta{ 0, 0 };

            if (m_wasMousePositionRegisteredLastFrame)
            {
                mouseDelta.x = Engine::Input::GetMouse().GetState().x - m_mousePositionLastFrame.x;
                mouseDelta.y = Engine::Input::GetMouse().GetState().y - m_mousePositionLastFrame.y;
            }

            if (Engine::Input::IsKeyDown(DirectX::Keyboard::A))
            {
                m_camera->Move(m_camera->GetTransform()->GetRight(), deltaTime * -moveSpeed);
            }

            if (Engine::Input::IsKeyDown(DirectX::Keyboard::D))
            {
                m_camera->Move(m_camera->GetTransform()->GetRight(), deltaTime * moveSpeed);
            }

            if (Engine::Input::IsKeyDown(DirectX::Keyboard::W))
            {
                m_camera->Move(m_camera->GetTransform()->GetForward(), deltaTime * moveSpeed);
            }

            if (Engine::Input::IsKeyDown(DirectX::Keyboard::S))
            {
                m_camera->Move(m_camera->GetTransform()->GetForward(), deltaTime * -moveSpeed);
            }

            if (Engine::Input::IsKeyDown(DirectX::Keyboard::Q))
            {
                m_camera->Move(m_camera->GetTransform()->GetUp(), deltaTime * -moveSpeed);
            }

            if (Engine::Input::IsKeyDown(DirectX::Keyboard::E))
            {
                m_camera->Move(m_camera->GetTransform()->GetUp(), deltaTime * moveSpeed);
            }

            m_camera->Move(m_camera->GetTransform()->GetForward(), m_scrollDelta * moveSpeed * 0.04f);
            m_camera->Rotate(mouseDelta.y * 0.003f, mouseDelta.x * 0.003f);

            m_mousePositionLastFrame.x = Engine::Input::GetMouse().GetState().x;
            m_mousePositionLastFrame.y = Engine::Input::GetMouse().GetState().y;
            m_scrollDelta = 0;
            m_wasMousePositionRegisteredLastFrame = Engine::Input::IsRightMouseButtonDown();
        }
        else
        {
            m_wasMousePositionRegisteredLastFrame = false;
        }
    }

    void EditorCameraController::OnMouseScroll(float delta)
    {
        m_scrollDelta += delta;
    }

}

