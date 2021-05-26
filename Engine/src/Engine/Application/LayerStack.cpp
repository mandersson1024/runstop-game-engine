#include "pch.h"
#include "LayerStack.h"
#include "Layer.h"

void Engine::LayerStack::PushLayer(std::shared_ptr<Layer> layer)
{
	layers_.emplace_back(layer);
}

void Engine::LayerStack::PopLayer(std::shared_ptr<Layer> layer)
{
	auto it = std::find(layers_.begin(), layers_.end(), layer);
	if (it != layers_.end())
	{
		layers_.erase(it);
	}
}
