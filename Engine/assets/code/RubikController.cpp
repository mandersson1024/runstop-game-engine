#include "pch.h"
#include "RubikController.h"
#include <numbers>
#include <DirectXMath.h>
#include "Engine\Scene\Transform.h"
#include "Engine\Platform\Input.h"
#include "Engine\Application\Application.h"
#include "Engine\Raycast\Raycast.h"
#include "Engine\Scene\Camera.h"
#include "Engine\Scene\Transform.h"
#include "Engine\Renderer\Renderer.h"

namespace CodeAssets
{

    void LogVector(const std::string& label, DirectX::XMVECTOR v)
    {
        ENGINE_LOG("{}: ({},{},{},{})",
            label, 
            DirectX::XMVectorGetX(v),
            DirectX::XMVectorGetY(v),
            DirectX::XMVectorGetZ(v),
            DirectX::XMVectorGetW(v)
        );
    }

    void LogInfoVector(const std::string& label, DirectX::XMVECTOR v)
    {
        ENGINE_LOG_INFO("{}: ({},{},{},{})",
            label,
            DirectX::XMVectorGetX(v),
            DirectX::XMVectorGetY(v),
            DirectX::XMVectorGetZ(v),
            DirectX::XMVectorGetW(v)
        );
    }

    DirectX::XMVECTOR Magnify(DirectX::XMVECTOR vec)
    {
        // Keep only biggest axis and normalize.
        float x = abs(DirectX::XMVectorGetX(vec));
        float y = abs(DirectX::XMVectorGetY(vec));
        float z = abs(DirectX::XMVectorGetZ(vec));

        if (x >= y && x >= z)
        {
            return DirectX::XMVector3Normalize({ DirectX::XMVectorGetX(vec), 0, 0 });
        }

        if (y >= x && y >= z)
        {
            return DirectX::XMVector3Normalize({ 0, DirectX::XMVectorGetY(vec), 0 });
        }

        return DirectX::XMVector3Normalize({ 0, 0, DirectX::XMVectorGetZ(vec) });
    }
    
    float MaxOfAbs(DirectX::XMVECTOR vec)
    {
        float x = abs(DirectX::XMVectorGetX(vec));
        float y = abs(DirectX::XMVectorGetY(vec));
        float z = abs(DirectX::XMVectorGetZ(vec));

        if (x >= y && x >= z)
        {
            return DirectX::XMVectorGetX(vec);
        }

        if (y >= x && y >= z)
        {
            return DirectX::XMVectorGetY(vec);
        }

        if (z >= x && z >= y)
        {
            return DirectX::XMVectorGetZ(vec);
        }

        return 0;
    }

    void RubikController::OnUpdate(float deltaTime)
    {
        if (tween_.running)
        {
            tween_.Update(deltaTime);

            GetRotator()->SetEulerAngles({
                    DirectX::XMVectorGetX(rotationAxis_) * tween_.value,
                    DirectX::XMVectorGetY(rotationAxis_) * tween_.value,
                    DirectX::XMVectorGetZ(rotationAxis_) * tween_.value
                });

            if (!tween_.running)
            {
                SnapAndRelease();
            }
        }
    }

    void RubikController::OnMouseDown(Engine::MouseDownEvent& evt)
    {
        const auto& mouse = Engine::Input::Get()->GetMouse().GetState();

        switch (evt.GetButton())
        {
        case 0:
            isLeftMouseDown_ = true;
            HandleLeftMouseDown(mouse.x, mouse.y);
            evt.handled = true;
            break;
        case 1:
            isRightMouseDown_ = true;
            HandleRightMouseDown(mouse.x, mouse.y);
            evt.handled = true;
            break;
        }
    }

    void RubikController::OnMouseUp(Engine::MouseUpEvent& evt)
    {
        const auto& mouse = Engine::Input::Get()->GetMouse().GetState();

        switch (evt.GetButton())
        {
        case 0:
            isLeftMouseDown_ = false;
            HandleLeftMouseUp(mouse.x, mouse.y);
            evt.handled = true;
            break;
        case 1:
            isRightMouseDown_ = false;
            HandleRightMouseUp(mouse.x, mouse.y);
            evt.handled = true;
            break;
        }
    }

    void RubikController::OnMouseMove(Engine::MouseMoveEvent& evt)
    {
        const auto& mouse = Engine::Input::Get()->GetMouse().GetState();

        if (isLeftMouseDown_)
        {
            HandleLeftMouseDrag(
                mouse.x, mouse.y,
                mouse.x - lastMouseX_,
                mouse.y - lastMouseY_
            );
        }

        if (isRightMouseDown_)
        {
            HandleRightMouseDrag(
                mouse.x, mouse.y,
                mouse.x - lastMouseX_,
                mouse.y - lastMouseY_
            );
        }

        lastMouseX_ = mouse.x;
        lastMouseY_ = mouse.y;

        evt.handled = true;
    }

    void RubikController::HandleLeftMouseDown(int x, int y)
    {
        if (tween_.running)
        {
            return;
        }

        isLeftMouseDragging_ = true;

        firstMouseX_ = x;
        firstMouseY_ = y;

        const auto camera = Engine::Application::GetRenderer().GetCamera();
        const auto ray = camera->ScreenPointToRay(x, y);

        const auto result = FindNearestRaycastHit(GetTransform(), ray);
        const auto hit = result.hit;

        if (hit.valid)
        {
            clickedPiece_ = result.transform;
            //m_clickedPiece->GetGameObject()->SetDebugInvertColors(true);

            lastPlanePoint_ = GetTransform()->InverseTransformPoint(hit.point);
            LogVector("m_lastCubePoint", lastPlanePoint_);

            const auto worldClickNormal = hit.normal;
            LogVector("worldClickNormal", worldClickNormal);

            cubeClickNormal_ = GetTransform()->InverseTransformDirection(worldClickNormal);
            LogVector("m_cubeClickNormal", cubeClickNormal_);

            cubeClickNormal_ = Magnify(cubeClickNormal_);
            LogVector("m_cubeClickNormal", cubeClickNormal_);

            worldDragPlane_ = Engine::PlaneCollider(worldClickNormal, hit.point);

            isClicked_ = true;
            rotationLocked_ = false;
            accumulatedRotation_ = 0;
            GetRotator()->SetOrientation({ 0, 0, 0, 0 });
        }
    }

    void RubikController::HandleRightMouseDown(int x, int y)
    {
        rightMouseDownPositionX_ = x;
        rightMouseDownPositionY_ = y;
        rightMouseDownOrientation_ = GetTransform()->GetOrientation();
    }

    void RubikController::HandleLeftMouseUp(int x, int y)
    {
        //GetGameObject()->SetDebugInvertColorsRecursively(false);

        if (isLeftMouseDragging_)
        {
            const float tau = 2 * static_cast<float>(std::numbers::pi);

            tween_ = RubikTween
            {
                .startValue = accumulatedRotation_,
                .endValue = roundf(accumulatedRotation_ / (tau / 4)) * (tau / 4),
                .duration = 0.3f,
            };

            tween_.Start();

            isLeftMouseDragging_ = false;
        }
    }

    void RubikController::HandleRightMouseUp(int x, int y)
    {
    }

    void RubikController::HandleLeftMouseDrag(int x, int y, int dx, int dy)
    {
        if (!isClicked_ || tween_.running)
        {
            return;
        }

        const auto dragDirection = GetDragDirection(lastPlanePoint_, x, y);

        if (!rotationLocked_)
        {
            float dragLengthSinceStart = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(
                DirectX::XMVectorSet(static_cast<float>(firstMouseX_), static_cast<float>(firstMouseY_), 0, 0),
                DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0, 0))));

            if (dragLengthSinceStart < 10)
            {
                ENGINE_LOG("anti-slip");
                return;
            }

            LogVector("initialDragDirection", dragDirection);

            rotationAxis_ = GetRotationAxis(dragDirection, cubeClickNormal_);
            LogVector("m_rotationAxis", rotationAxis_);

            BuildChunk();
            rotationLocked_ = true;
        }

        float dragLength = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(
            DirectX::XMVectorSet(static_cast<float>(lastMouseX_), static_cast<float>(lastMouseY_), 0, 0 ),
            DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0, 0 ))));

        float angle = MaxOfAbs(dragDirection) * dragLength * rotationSensitivity_;
        accumulatedRotation_ += angle;

        GetRotator()->SetEulerAngles({
                DirectX::XMVectorGetX(rotationAxis_) * accumulatedRotation_,
                DirectX::XMVectorGetY(rotationAxis_) * accumulatedRotation_,
                DirectX::XMVectorGetZ(rotationAxis_) * accumulatedRotation_
            });
    }

    void RubikController::HandleRightMouseDrag(int x, int y, int dx, int dy)
    {
        if (isLeftMouseDragging_)
        {
            return;
        }

        const float dxSinceStart = static_cast<float>(rightMouseDownPositionX_ - x);
        const float dySinceStart = static_cast<float>(rightMouseDownPositionY_ - y);
        const DirectX::XMVECTOR axis = { dySinceStart, dxSinceStart, 0, 0 };
        const float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(axis));
        const bool hasMoved = abs(distance) > 0.1f;
        const auto rotation = hasMoved ? DirectX::XMQuaternionRotationAxis(axis, rotationSensitivity_ * distance) : DirectX::XMQuaternionIdentity();
        const auto orientation = DirectX::XMQuaternionMultiply(rightMouseDownOrientation_, rotation);
        GetTransform()->SetOrientation(orientation);
    }

    std::shared_ptr<Engine::Transform> RubikController::GetRotator() const
    {
        return GetGameObject()->GetTransform()->FindChildWithName("rotator");
    }

    std::shared_ptr<Engine::Transform> RubikController::GetTransform() const
    {
        return GetGameObject()->GetTransform();
    }

    const std::vector<std::shared_ptr<Engine::Transform>>& RubikController::GetChildren() const
    {
        return GetGameObject()->GetTransform()->GetChildren();
    }

    std::vector<std::shared_ptr<Engine::Transform>> RubikController::FindAllPiecesAtX(float x)
    {
        std::vector<std::shared_ptr<Engine::Transform>> result{};
        
        for (const auto child : GetChildren())
        {
            if (child != GetRotator() && DirectX::XMVectorGetX(child->GetPosition()) == x)
            {
                result.push_back(child);
            }
        }

        return result;
    }

    std::vector<std::shared_ptr<Engine::Transform>> RubikController::FindAllPiecesAtY(float y)
    {
        std::vector<std::shared_ptr<Engine::Transform>> result{};

        for (const auto child : GetChildren())
        {
            if (child != GetRotator() && DirectX::XMVectorGetY(child->GetPosition()) == y)
            {
                result.push_back(child);
            }
        }

        return result;
    }

    std::vector<std::shared_ptr<Engine::Transform>> RubikController::FindAllPiecesAtZ(float z)
    {
        std::vector<std::shared_ptr<Engine::Transform>> result{};

        for (const auto child : GetChildren())
        {
            if (child != GetRotator() && DirectX::XMVectorGetZ(child->GetPosition()) == z)
            {
                result.push_back(child);
            }
        }

        return result;
    }

    RubikController::RaycastResult RubikController::FindNearestRaycastHit(std::shared_ptr<Engine::Transform> root, const Engine::Ray& ray)
    {
        RaycastResult nearest
        {
            .hit =
            {
                .valid = false,
                .distance = std::numeric_limits<float>::max()
            }
        };

        if (root->HasCollider())
        {
            const auto hit = root->GetCollider()->Raycast(ray);

            if (hit.valid)
            {
                nearest.hit = hit;
                nearest.transform = root;
            }
        }

        for (auto child : root->GetChildren())
        {
            const auto result = FindNearestRaycastHit(child, ray);
            
            if (result.hit.valid && result.hit.distance < nearest.hit.distance)
            {
                nearest = result;
            }
        }

        return nearest;
    }

    DirectX::XMVECTOR RubikController::GetDragDirection(DirectX::XMVECTOR cubeReferencePoint, int mouseX, int mouseY)
    {
        // find mouse position on drag plane in world
        const auto camera = Engine::Application::GetRenderer().GetCamera();
        const auto ray = camera->ScreenPointToRay(mouseX, mouseY);
        auto hit = worldDragPlane_.Raycast(ray);

        if (!hit.valid)
        {
            //
            // HACK:
            // We have dragged outside the plane (when the plane is in such a steep angle that the horizon is on-screen).
            // Then cast a ray backwards instead. This works good enough in the current conditions, but is not stable.
            //

            const auto backRay = Engine::Ray{ ray.origin, DirectX::XMVectorSubtract({ 0, 0, 0, 0 }, ray.direction) };
            hit = worldDragPlane_.Raycast(backRay);
        }

        if (hit.valid)
        {
            // transform point to cube coordinates
            lastPlanePoint_ = GetTransform()->InverseTransformDirection(hit.point);

            const auto dragDirection = DirectX::XMVectorSubtract(cubeReferencePoint, lastPlanePoint_);
            const auto magnifiedDragDirection = Magnify(dragDirection);

            return magnifiedDragDirection;
        }
        else
        {
            // If the backwards ray also misses, then the ray is parallell to the plane.

            return { 0, 0, 0, 0 };
        }
    }

    DirectX::XMVECTOR RubikController::GetRotationAxis(DirectX::XMVECTOR dragDirection, DirectX::XMVECTOR normal)
    {
        const float dragX = DirectX::XMVectorGetX(dragDirection);
        const float dragY = DirectX::XMVectorGetY(dragDirection);
        const float dragZ = DirectX::XMVectorGetZ(dragDirection);

        const float pieceX = DirectX::XMVectorGetX(normal);
        const float pieceY = DirectX::XMVectorGetY(normal);
        const float pieceZ = DirectX::XMVectorGetZ(normal);

        float signPieceX = pieceX > 0.f ? 1.f : -1.f;
        float signPieceY = pieceY > 0.f ? 1.f : -1.f;
        float signPieceZ = pieceZ > 0.f ? 1.f : -1.f;

        if (pieceX != 0)
        {
            if (dragY != 0)
            {
                return DirectX::XMVectorSet(0, 0, -signPieceX, 0);
            }

            if (dragZ != 0)
            {
                return DirectX::XMVectorSet(0, signPieceX, 0, 0);
            }

            return DirectX::XMVectorZero();
        }

        if (pieceY != 0)
        {
            if (dragX != 0)
            {
                return DirectX::XMVectorSet(0, 0, signPieceY, 0);
            }

            if (dragZ != 0)
            {
                return DirectX::XMVectorSet(-signPieceY, 0, 0, 0);
            }

            return DirectX::XMVectorZero();
        }

        if (pieceZ != 0)
        {
            if (dragX != 0)
            {
                return DirectX::XMVectorSet(0, -signPieceZ, 0, 0);
            }

            if (dragY != 0)
            {
                return DirectX::XMVectorSet(signPieceZ, 0, 0, 0);
            }

            return DirectX::XMVectorZero();
        }

        return DirectX::XMVectorZero();
    }

    void RubikController::BuildChunk()
    {
        const auto hitPiecePosition = clickedPiece_->GetPosition();

        bool axisX = abs(DirectX::XMVectorGetX(rotationAxis_)) == 1;
        bool axisY = abs(DirectX::XMVectorGetY(rotationAxis_)) == 1;
        bool axisZ = abs(DirectX::XMVectorGetZ(rotationAxis_)) == 1;

        if (axisX)
        {
            selectedPieces_ = FindAllPiecesAtX(DirectX::XMVectorGetX(hitPiecePosition));
        }
        else if (axisY)
        {
            selectedPieces_ = FindAllPiecesAtY(DirectX::XMVectorGetY(hitPiecePosition));
        }
        else if (axisZ)
        {
            selectedPieces_ = FindAllPiecesAtZ(DirectX::XMVectorGetZ(hitPiecePosition));
        }

        for (auto piece : selectedPieces_)
        {
            //
            // Detach the chunk pieces from 'Root' and attach them to 'Rotator'
            //

            //piece->GetGameObject()->SetDebugInvertColors(true);
            Engine::Transform::DetachChildFromParent(piece->GetParent(), piece, true);
            Engine::Transform::AttachChildToParent(GetRotator(), piece, true);
        }
    }

    void RubikController::SnapAndRelease()
    {
        //
        // Snap chunk
        //

        auto angles = GetRotator()->GetEulerAngles();
        const float tau = 2 * static_cast<float>(std::numbers::pi);

        float snapRotation = roundf(accumulatedRotation_ / (tau / 4)) * (tau / 4);

        GetRotator()->SetEulerAngles({
                DirectX::XMVectorGetX(rotationAxis_) * snapRotation,
                DirectX::XMVectorGetY(rotationAxis_) * snapRotation,
                DirectX::XMVectorGetZ(rotationAxis_) * snapRotation
            });

        //
        // Release chunk pieces from 'Rotator' and attach to 'Root'
        //

        for (auto piece : selectedPieces_)
        {
            Engine::Transform::DetachChildFromParent(piece->GetParent(), piece, true);
            Engine::Transform::AttachChildToParent(GetTransform(), piece, true);

            //
            // Clean up any rounding errors
            //

            piece->SetPosition(DirectX::XMVectorRound(piece->GetPosition()));
        }

    }

}
