#pragma once
#include <string>
#include <memory>

namespace Engine
{
    class MaterialAsset;
}

namespace Editor
{

 class ProjectGui
    {
    public:
        void Draw();

    private:
        void DrawGraphics();
        void DrawScenes();
        void DrawMaterials();
        void DrawMaterial(const std::string& id, std::shared_ptr<Engine::MaterialAsset> asset);
        void DrawMaterialNotLoaded(const std::string& id);
    };

}

