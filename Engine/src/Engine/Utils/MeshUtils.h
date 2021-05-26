#pragma once
#include <memory>

namespace Engine
{

    class Mesh;

    std::shared_ptr<Mesh> CreateQuadMesh();
    std::shared_ptr<Mesh> CreateSkyboxMesh();

}

