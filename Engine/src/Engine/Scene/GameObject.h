#pragma once
#include <string>
#include <vector>


namespace Engine
{
	class Transform;
    class GameObjectController;
	class Mesh;
	class Shader;
	class Texture2D;
	class SamplerState;
	class BlendState;
	class RasterizerState;


	class GameObject
	{
	public:
        std::shared_ptr<Transform> GetTransform() const { return m_transform; }
        //const std::string& GetName() const { return m_name; }
        std::string GetModelId() const { return m_modelId; }
        const std::vector<std::string>& GetMaterialIds() const { return materialIds_; }
        std::vector<std::shared_ptr<GameObject>> GetChildren() const;

		bool IsActive() const { return m_active; }
        bool HasModel() const { return !m_modelId.empty(); }

        void SetTransform(std::shared_ptr<Transform> transform) { m_transform = transform; }
		//void SetName(const std::string& name) { m_name = name; }
		void SetModelId(const std::string& modelId) { m_modelId = modelId; }
		void AddMaterialId(const std::string& id) { materialIds_.push_back(id); }
		void SetActive(bool active) { m_active = active; }

		bool HasController() const { return m_controller != nullptr; }
        void SetController(std::shared_ptr<GameObjectController> controller) { m_controller = controller; }
		std::shared_ptr<GameObjectController> GetController() const { return m_controller; }

		void SetDebugInvertColors(bool invert) { m_debugInvertColors = invert; }
        bool GetDebugInvertColors() const { return m_debugInvertColors; }
        void SetDebugInvertColorsRecursively(bool invert)
		{ 
			m_debugInvertColors = invert; 

			for (auto child : GetChildren())
			{
				child->SetDebugInvertColorsRecursively(invert);
			}
		}
		

        void OnUpdateControllers(float deltaTime);
        void OnUpdate(float deltaTime);

	private:
		std::shared_ptr<Transform> m_transform;
		std::string m_modelId;
		std::vector<std::string> materialIds_;
		std::shared_ptr<GameObjectController> m_controller;
		bool m_active = true;
		bool m_debugInvertColors = false;
	};

}
