#pragma once

namespace Engine
{

    class ShaderCompiler
    {
    public:
        ShaderCompiler() = delete;

        static Microsoft::WRL::ComPtr<ID3DBlob> CompileVertexShader(const std::wstring& srcFile);
        static Microsoft::WRL::ComPtr<ID3DBlob> CompilePixelShader(const std::wstring& srcFile);
        static Microsoft::WRL::ComPtr<ID3DBlob> CompileComputeShader(const std::wstring& srcFile);

    private:
        static Microsoft::WRL::ComPtr<ID3DBlob> Compile(const std::wstring& srcFile, std::string profile);
    };

}

