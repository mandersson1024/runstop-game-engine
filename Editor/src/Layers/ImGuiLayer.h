#pragma once
#include <memory>
#include "Engine\Application\Layer.h"

namespace Engine
{
	class Window;
	class Event;
}

namespace Editor
{
	class ToolsGui;
	class ProjectGui;
	class SceneGui;

	class ImGuiLayer : public Engine::Layer
	{
	public:
		ImGuiLayer(Engine::Window& window);
		~ImGuiLayer();

		void OnEvent(Engine::Event& evt) override;

		void Begin();
		void End();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;

	private:
		Engine::Window& window_;
        std::unique_ptr<ToolsGui> toolsGui_;
        std::unique_ptr<ProjectGui> projectGui_;
        std::unique_ptr<SceneGui> sceneGui_;
	};

}
