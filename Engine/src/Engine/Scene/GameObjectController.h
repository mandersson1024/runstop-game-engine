#pragma once
#include <functional>
#include "..\Assets\AssetLibrary.h"

namespace Engine
{

    class GameObject;

    class GameObjectController
    {
    public:
        using FactoryFunction = std::function<std::shared_ptr<GameObjectController>()>;

        GameObjectController(const std::string name)
            : name_(name)
        {
        }

        virtual ~GameObjectController() 
        {
        }

        virtual void OnMouseDown(MouseDownEvent& event) {}
        virtual void OnMouseUp(MouseUpEvent& event) {}
        virtual void OnMouseMove(MouseMoveEvent& event) {}

        virtual void OnUpdate(float deltaTime) {}

        template <typename T>
        static std::function<std::shared_ptr<GameObjectController>()> GetFactory()
        {
            return []() { return std::make_shared<T>(); };
        }

        static AssetLibrary<FactoryFunction>& GetLibrary() { return library_; }

        void SetGameObject(std::shared_ptr<GameObject> gameObject) { gameObject_ = gameObject; }
        std::shared_ptr<GameObject> GetGameObject() const { return gameObject_; }

        std::string GetName() const { return name_; }

    private:
        static AssetLibrary<FactoryFunction> library_;
        std::string name_;
        std::shared_ptr<GameObject> gameObject_;
    };

}
