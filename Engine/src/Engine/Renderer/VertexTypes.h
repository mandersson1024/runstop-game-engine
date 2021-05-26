#pragma once
#include <string>
#include <vector>
#include <d3d11.h>

namespace Engine
{

    enum class VertexType
    {
        Undefined,
        Minimal,
        Default,
    };

    VertexType VertexTypeFromString(const std::string& str);

    uint32_t GetVertexTypeSize(VertexType type);

    const std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements(VertexType type);

    struct VertexTypeMinimal
    {
        struct 
        {
            struct { float x, y, z; } Position;
        } Data;

        static uint32_t GetSize() { return sizeof(Data); }

        static const std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements()
        {
            return
            {
                { "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
        }
    };

    struct VertexTypeDefault
    {
        struct 
        {
            struct { float x, y, z; } Position;
            struct { float x, y, z; } Normal;
            struct { float x, y, z; } Tangent;
            struct { float x, y, z; } Bitangent;
            struct { float u, v; } TexCoord;
        } Data;

        static uint32_t GetSize() { return sizeof(Data); }

        static const std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputElements()
        {
            return
            {
                { "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "BINORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TexCoord",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
        }
    };
}