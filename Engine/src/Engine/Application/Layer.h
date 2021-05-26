#pragma once
#include <string>
#include <windows.h>
#include "Engine/Application/Event.h"

namespace Engine
{

	class Layer
	{
	public:
		Layer(const std::string& name);
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) { UNREFERENCED_PARAMETER(deltaTime); }
		virtual void OnEvent(Event&	evt) { UNREFERENCED_PARAMETER(evt); }

		std::string GetName() const { return name_; }

	protected:
		std::string name_;
	};

}
