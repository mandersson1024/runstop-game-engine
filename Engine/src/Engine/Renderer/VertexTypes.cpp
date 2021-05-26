#include "pch.h"
#include "VertexTypes.h"
#include "../EngineAssert.h"

namespace Engine
{

    Engine::VertexType VertexTypeFromString(const std::string& str)
    {
        if (str == "minimal") { return VertexType::Minimal; }
        if (str == "default") { return VertexType::Default; }

        return VertexType::Undefined;
    }

    uint32_t GetVertexTypeSize(VertexType type)
    {
        switch (type)
        {
            case VertexType::Minimal: return VertexTypeMinimal::GetSize();
            case VertexType::Default: return VertexTypeDefault::GetSize();
        }

        ENGINE_ASSERT(false, "Unknown vertex type");

        return 0;
    }

    const std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(VertexType type)
    {
        switch (type)
        {
            case VertexType::Minimal: return VertexTypeMinimal::GetInputElements();
            case VertexType::Default: return VertexTypeDefault::GetInputElements();
        }

        ENGINE_ASSERT(false, "Unknown vertex type");

        return {};
    }

}

