#include "pch.h"
#include "Shader.h"
#include "Engine/Renderer/Graphics.h"
#include "Engine/Renderer/ShaderCompiler.h"

namespace Engine
{

    Shader::Shader(const std::filesystem::path& vertexShaderFilePath, const std::filesystem::path& pixelShaderFilePath, VertexType vertexType)
        : vertexShaderFilePath_(vertexShaderFilePath)
        , pixelShaderFilePath_(pixelShaderFilePath)
        , vertexType_(vertexType)
    {
        CompileVertexShader();
        CompilePixelShader();
    }

    void Shader::CompileVertexShader()
    {
        ENGINE_LOG_INFO("Compiling vertex shader: '{}'", vertexShaderFilePath_.generic_string());

        vertexShader_ = nullptr;

        if (const auto blob = ShaderCompiler::CompileVertexShader(vertexShaderFilePath_))
        {
            Graphics::GetDevice()->CreateVertexShader(
                blob->GetBufferPointer(),
                blob->GetBufferSize(),
                nullptr,
                &vertexShader_);

            const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements = GetInputElements(vertexType_);

            Graphics::GetDevice()->CreateInputLayout(
                inputElements.data(),
                static_cast<UINT>(inputElements.size()),
                blob->GetBufferPointer(),
                blob->GetBufferSize(),
                &inputLayout_);
        }
    }

    void Shader::CompilePixelShader()
    {
        ENGINE_LOG_INFO("Compiling pixel shader: '{}'", pixelShaderFilePath_.generic_string());

        pixelShader_ = nullptr;

        if (const auto blob = ShaderCompiler::CompilePixelShader(pixelShaderFilePath_))
        {
            Graphics::GetDevice()->CreatePixelShader(
                blob->GetBufferPointer(),
                blob->GetBufferSize(),
                nullptr,
                &pixelShader_);
        }
    }

    void Shader::Bind() const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            Graphics::GetContext()->VSSetShader(vertexShader_.Get(), nullptr, 0);
            Graphics::GetContext()->PSSetShader(pixelShader_.Get(), nullptr, 0);
            Graphics::GetContext()->IASetInputLayout(inputLayout_.Get());
        );
    }

    bool Shader::IsSuccessfullyCompiled() const
{
        return vertexShader_ && pixelShader_;
    }

    ComputeShader::ComputeShader(const std::filesystem::path& filePath)
        : filepath_(filePath)
    {
        Compile();
    }

    void ComputeShader::Compile()
    {
        shader_ = nullptr;
        ENGINE_LOG_INFO("Compiling compute shader: '{}'", filepath_.generic_string());

        if (const auto blob = ShaderCompiler::CompileComputeShader(filepath_))
        {
            Graphics::GetDevice()->CreateComputeShader(
                blob->GetBufferPointer(),
                blob->GetBufferSize(),
                nullptr,
                &shader_);
        }
    }

    bool ComputeShader::IsSuccessfullyCompiled() const
    {
        return shader_ != nullptr;
    }

    void ComputeShader::Bind() const
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(Graphics::GetContext()->CSSetShader(shader_.Get(), nullptr, 0));
    }

}

