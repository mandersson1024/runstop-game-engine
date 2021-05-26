#include "pch.h"
#include "ShaderCompiler.h"
#include <d3dcompiler.h>

namespace Engine
{

    Microsoft::WRL::ComPtr<ID3DBlob> ShaderCompiler::Compile(const std::wstring& srcFile, std::string profile)
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
        flags |= D3DCOMPILE_DEBUG;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3DCompileFromFile(
            srcFile.c_str(),
            NULL,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main",
            profile.c_str(),
            flags,
            0,
            &shaderBlob,
            &errorBlob
        );

        if (FAILED(hr))
        {
            ENGINE_LOG_ERROR("Error compiling shader: {}", (char*)errorBlob->GetBufferPointer());
        }

        return shaderBlob;
    }

    Microsoft::WRL::ComPtr<ID3DBlob> ShaderCompiler::CompileVertexShader(const std::wstring& srcFile)
    {
        return Compile(srcFile.c_str(), "vs_5_0");
    }

    Microsoft::WRL::ComPtr<ID3DBlob> ShaderCompiler::CompilePixelShader(const std::wstring& srcFile)
    {
        return Compile(srcFile.c_str(), "ps_5_0");
    }

    Microsoft::WRL::ComPtr<ID3DBlob> ShaderCompiler::CompileComputeShader(const std::wstring& srcFile)
    {
        auto blob = Compile(srcFile.c_str(), "cs_5_0");
        ENGINE_ASSERT(blob != nullptr, "Error compiling compute shader");
        return blob;
    }

}

