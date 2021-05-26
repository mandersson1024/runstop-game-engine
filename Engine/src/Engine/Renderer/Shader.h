#pragma once
#include <vector>
#include <filesystem>
#include <d3d11.h>
#include "VertexTypes.h"

namespace Engine
{

	class Shader
	{
	public:
        Shader(const std::filesystem::path& vertexShaderFilePath, const std::filesystem::path& pixelShaderFilePath, VertexType vertexType);

		void CompileVertexShader();
        void CompilePixelShader();
        bool IsSuccessfullyCompiled() const;
		
		void Bind() const;

        const std::filesystem::path& GetVertexShaderFilePath() const { return vertexShaderFilePath_; }
        const std::filesystem::path& GetPixelShaderFilePath() const { return pixelShaderFilePath_; }

	private:
        std::filesystem::path vertexShaderFilePath_;
        std::filesystem::path pixelShaderFilePath_;

        VertexType vertexType_;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
	};


    class ComputeShader
    {
    public:
        ComputeShader(const std::filesystem::path& filePath);

        void Compile();
        bool IsSuccessfullyCompiled() const;

        void Bind() const;

        const std::filesystem::path& GetFilePath() const { return filepath_; }

    private:
        std::filesystem::path filepath_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> shader_;
    };

}

