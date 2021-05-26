#pragma once
#include "Engine\Scene\GameObjectController.h"
#include "Engine\Application\MouseEvent.h"
#include "Engine\Raycast\Raycast.h"
#include "Engine\Scene\GameObject.h"

namespace Engine
{
    class Transform;
}

namespace CodeAssets
{

    struct RubikTween
    {
        float startValue;
        float endValue;
        float value;
        float duration;
        float elapsed;
        bool running = false;

        void Start()
        {
            elapsed = 0;
            value = startValue;
            running = true;
        }

        void Update(float deltaTime)
        {
            elapsed += deltaTime;

            if (elapsed <= 0)
            {
                value = startValue;
                return;
            }

            if (elapsed >= duration)
            {
                running = false;
                value = endValue;
                return;
            }

            float t = elapsed / duration;

            t = (t -= 1) * t * t + 1; // ease out cubic

            value = startValue + (endValue - startValue) * t;
        }
    };


    class RubikController : public Engine::GameObjectController
    {
    public:
        RubikController() : GameObjectController("rubik_controller")
        {
        }

        void OnMouseDown(Engine::MouseDownEvent& evt) override;
        void OnMouseUp(Engine::MouseUpEvent& evt) override;
        void OnMouseMove(Engine::MouseMoveEvent& evt) override;

        void OnUpdate(float deltaTime) override;

    private:
        void HandleLeftMouseDown(int x, int y);
        void HandleRightMouseDown(int x, int y);
        void HandleLeftMouseUp(int x, int y);
        void HandleRightMouseUp(int x, int y);
        void HandleLeftMouseDrag(int x, int y, int dx, int dy);
        void HandleRightMouseDrag(int x, int y, int dx, int dy);

        std::shared_ptr<Engine::Transform> GetRotator() const;
        std::shared_ptr<Engine::Transform> GetTransform() const;
        const std::vector<std::shared_ptr<Engine::Transform>>& GetChildren() const;

        std::vector<std::shared_ptr<Engine::Transform>> FindAllPiecesAtX(float x);
        std::vector<std::shared_ptr<Engine::Transform>> FindAllPiecesAtY(float y);
        std::vector<std::shared_ptr<Engine::Transform>> FindAllPiecesAtZ(float z);

        struct RaycastResult
        {
            Engine::RaycastHit hit;
            std::shared_ptr<Engine::Transform> transform;
        };

        RaycastResult FindNearestRaycastHit(std::shared_ptr<Engine::Transform> root, const Engine::Ray& ray);
        DirectX::XMVECTOR GetDragDirection(DirectX::XMVECTOR cubeReferencePoint, int mouseX, int mouseY);
        DirectX::XMVECTOR GetRotationAxis(DirectX::XMVECTOR dragDirection, DirectX::XMVECTOR normal);
        void BuildChunk();
        void SnapAndRelease();

    private:
        int lastMouseX_ = 0;
        int lastMouseY_ = 0;
        int rightMouseDownPositionX_ = 0;
        int rightMouseDownPositionY_ = 0;
        bool isLeftMouseDown_ = false;
        bool isRightMouseDown_ = false;

        bool isLeftMouseDragging_ = false;
        int firstMouseX_ = 0;
        int firstMouseY_ = 0;
        DirectX::XMVECTOR lastPlanePoint_;
        DirectX::XMVECTOR cubeClickNormal_;
        Engine::PlaneCollider worldDragPlane_;
        bool isClicked_ = false;
        bool rotationLocked_ = false;
        DirectX::XMVECTOR rotationAxis_;

        DirectX::XMVECTOR rightMouseDownOrientation_;
        float rotationSensitivity_ = 0.006f;

        std::shared_ptr<Engine::Transform> clickedPiece_ = nullptr;
        float accumulatedRotation_ = 0;
        std::vector<std::shared_ptr<Engine::Transform>> selectedPieces_{};

        RubikTween tween_;
    };
}
