#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <wrl.h>
#include <d3d11_3.h>
#include <DirectXMath.h>
#include "GraphicsSettings.h"

namespace Engine
{

    class Graphics
    {
    public:
        Graphics(const Graphics&) = delete;
        Graphics& operator= (const Graphics&) = delete;
        Graphics(const GraphicsSettings& settings, HWND hWnd);

        static void Init(const GraphicsSettings& settings, HWND hWnd);
        static bool IsInitialized() { return instance_ != nullptr; }

        static GraphicsSettings GetSettings() { return instance_->settings_; }
        static void ChangeSettings(const GraphicsSettings& settings);
        static const std::vector<AntiAliasingCapability>& GetAntiAliasingCapabilities() { return instance_->antiAliasingCapabilities_; }

        static ID3D11Device* GetDevice() { return instance_->device_.Get(); }
        static ID3D11DeviceContext* GetContext() { return instance_->context_.Get(); }
        static ID3D11RenderTargetView* GetRenderTargetView() { return instance_->renderTargetView_.Get(); }
        static ID3D11DepthStencilView* GetDepthStencilView() { return instance_->depthStencilView_.Get(); }
        static D3D11_VIEWPORT GetScreenViewport() { return instance_->screenViewport_; }
        static ID3D11ShaderResourceView* GetShadowMapShaderResourceView();
        static ID3D11Texture2D* GetShadowMapShaderRenderTarget();

        static void SetMSAARenderTarget();
        static void SetShadowMapRenderTarget();
        static void SetShadowMapShaderResource();
        static void ReleaseShadowMapShaderResource();
        static void SetScreenViewport();
        static void SetShadowMapViewport();
        static void ResolveMSAARenderTarget();
        static void ClearMSAARenderTargetView(DirectX::XMVECTOR color);
        static void ClearMSAADepthStencilView();
        static void ClearShadowMapDepthStencilView();

        static int GetWidth();
        static int GetHeight();

        static void Present();
        static void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int baseVertexLocation);

    private:
        void Setup();
        void CheckAntiAliasingCapabilities();
        void CreateWindowSizeDependentResources();

        void CreateSwapChain(uint32_t width, uint32_t height);
        void CreateRenderTarget();
        void CreateRenderTargetView();
        void CreateDepthStencil(uint32_t width, uint32_t height);
        void CreateDepthStencilView();
        void CreateScreenViewport(uint32_t width, uint32_t height);
        void CreateMSAARenderTarget(uint32_t width, uint32_t height);
        void CreateMSAARenderTargetView();
        void CreateMSAADepthStencilView(uint32_t width, uint32_t height);
        void CreateShadowMapRenderTarget(uint32_t width, uint32_t height);
        void CreateShadowMapDepthStencilView();
        void CreateShadowMapShaderResourceView();
        void CreateShadowMapViewport(uint32_t width, uint32_t height);

        DXGI_FORMAT GetBackBufferFormat() const { return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; }
        DXGI_FORMAT GetDepthBufferFormat() const { return DXGI_FORMAT_D32_FLOAT; }

    private:
        static std::unique_ptr<Graphics> instance_;

        HWND hwnd_;
        GraphicsSettings settings_;
        std::vector<AntiAliasingCapability> antiAliasingCapabilities_;

        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory_;
        Microsoft::WRL::ComPtr<ID3D11Device1> device_;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context_;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain_;
        Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> annotation_;
    
        Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTarget_;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil_;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> msaaRenderTarget_;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> msaaRenderTargetView_;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> msaaDepthStencilView_;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowMapRenderTarget_;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowMapDepthStencilView_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowMapShaderResourceView_;

        D3D11_VIEWPORT screenViewport_;
        D3D11_VIEWPORT shadowMapViewport_;

    public:
        static std::mutex contextMutex_;
    };
}

// ID3D11DeviceContext is not thread-safe
#define WITH_GUARDED_GRAPHICS_CONTEXT(code) { const std::lock_guard<std::mutex> lock(::Engine::Graphics::contextMutex_); code; }

