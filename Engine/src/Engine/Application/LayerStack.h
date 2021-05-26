#pragma once
#include <memory>
#include <vector>

namespace Engine
{
	class Layer;

	class LayerStack
	{
	public:
		void PushLayer(std::shared_ptr<Layer> layer);
		void PopLayer(std::shared_ptr<Layer> layer);

		std::vector<std::shared_ptr<Layer>>::iterator begin() { return layers_.begin(); }
		std::vector<std::shared_ptr<Layer>>::iterator end() { return layers_.end(); }

	private:
		std::vector<std::shared_ptr<Layer>> layers_;
	};

}


