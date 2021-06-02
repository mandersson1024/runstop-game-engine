#include "pch.h"
#include "Graphics.h"
#include <winerror.h>
#include "Engine/Logger.h"
#include "../EngineAssert.h"

namespace Engine
{

    std::unique_ptr<Graphics> Graphics::instance_;
    std::mutex Graphics::contextMutex_;

    void Graphics::Init(const GraphicsSettings& settings, HWND hWnd)
    {
        instance_ = std::make_unique<Graphics>(settings, hWnd);
    }

    Graphics::Graphics(const GraphicsSettings& settings, HWND hWnd)
        : settings_(settings)
        , hwnd_(hWnd)
        , screenViewport_{}
    {
        ENGINE_LOG("Creating Graphics");

        Setup();
        CheckAntiAliasingCapabilities();
        CreateWindowSizeDependentResources();
    }
    
    void Graphics::Setup()
    {
        DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf())));
        Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
        DX::ThrowIfFailed(dxgiFactory_.As(&factory6));

        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        uint32_t adapterIndex = 0;
        DX::ThrowIfFailed(factory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())));

        DXGI_ADAPTER_DESC1 adapterDesc;
        DX::ThrowIfFailed(adapter->GetDesc1(&adapterDesc));
        ENGINE_ASSERT((adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0, "This is a software adapter");

#ifdef _DEBUG
        wchar_t buff[256] = {};
        swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, adapterDesc.VendorId, adapterDesc.DeviceId, adapterDesc.Description);
        OutputDebugStringW(buff);
#endif

        static const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        Microsoft::WRL::ComPtr<ID3D11Device> device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            DX::ThrowIfFailed(D3D11CreateDevice(
                adapter.Get(),
                D3D_DRIVER_TYPE_UNKNOWN,
                NULL,
                creationFlags,
                featureLevels,
                1,
                D3D11_SDK_VERSION,
                device.GetAddressOf(),
                NULL,
                context.GetAddressOf()));
        );

#ifndef NDEBUG
        Microsoft::WRL::ComPtr<ID3D11Debug> d3dDebug;
        if (SUCCEEDED(device.As(&d3dDebug)))
        {
            Microsoft::WRL::ComPtr<ID3D11InfoQueue> d3dInfoQueue;
            if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
            {
#ifdef _DEBUG
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
                d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
                D3D11_MESSAGE_ID hide[] =
                {
                    D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                };
                D3D11_INFO_QUEUE_FILTER filter = {};
                filter.DenyList.NumIDs = _countof(hide);
                filter.DenyList.pIDList = hide;
                d3dInfoQueue->AddStorageFilterEntries(&filter);
            }
        }
#endif

        DX::ThrowIfFailed(device.As(&device_));

        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            DX::ThrowIfFailed(context.As(&context_));
            DX::ThrowIfFailed(context.As(&annotation_));
        );
    }

    ID3D11ShaderResourceView* Graphics::GetShadowMapShaderResourceView()
    {

        if (!instance_->shadowMapShaderResourceView_)
        {
            return nullptr;
        }
        else
        {
            return instance_->shadowMapShaderResourceView_.Get();
        }
    }

    ID3D11Texture2D* Graphics::GetShadowMapShaderRenderTarget()
    {
        if (!instance_->shadowMapRenderTarget_)
        {
            return nullptr;
        }
        else
        {
            return instance_->shadowMapRenderTarget_.Get();
        }
    }

    void Graphics::ChangeSettings(const GraphicsSettings& settings)
    {
        instance_->settings_ = settings;

        RECT rect;
        GetClientRect(instance_->hwnd_, &rect);
        uint32_t width = rect.right - rect.left;
        uint32_t height = rect.bottom - rect.top;

        instance_->CreateMSAARenderTarget(width, height);
        instance_->CreateMSAARenderTargetView();
        instance_->CreateMSAADepthStencilView(width, height);
    }

    void Graphics::SetMSAARenderTarget()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->OMSetRenderTargets(1, instance_->msaaRenderTargetView_.GetAddressOf(), instance_->msaaDepthStencilView_.Get()));
    }

    void Graphics::SetShadowMapRenderTarget()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->OMSetRenderTargets(0, NULL, instance_->shadowMapDepthStencilView_.Get()));
    }

    void Graphics::SetShadowMapShaderResource()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->PSSetShaderResources(1, 1, instance_->shadowMapShaderResourceView_.GetAddressOf()));
    }

    void Graphics::ReleaseShadowMapShaderResource() 
    {
        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            ID3D11ShaderResourceView * pNull = NULL;
        instance_->context_->PSSetShaderResources(1, 1, &pNull);
        );
    }

    void Graphics::SetScreenViewport()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->RSSetViewports(1, &instance_->screenViewport_));
    }

    void Graphics::SetShadowMapViewport()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->RSSetViewports(1, &instance_->shadowMapViewport_));
    }
    
    void Graphics::ResolveMSAARenderTarget()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            instance_->annotation_->BeginEvent(L"ResolveMSAARenderTarget");
            instance_->context_->ResolveSubresource(instance_->renderTarget_.Get(), 0, instance_->msaaRenderTarget_.Get(), 0, instance_->GetBackBufferFormat());
            instance_->annotation_->EndEvent();
        );
    }

    void Graphics::ClearMSAARenderTargetView(DirectX::XMVECTOR color)
    {
        const auto linear = GammaToLinear(color);
        const DirectX::XMVECTORF32 f32 = 
        {
            DirectX::XMVectorGetX(linear),
            DirectX::XMVectorGetY(linear),
            DirectX::XMVectorGetZ(linear),
            DirectX::XMVectorGetW(linear)
        };

        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->ClearRenderTargetView(instance_->msaaRenderTargetView_.Get(), f32));
    }

    void Graphics::ClearMSAADepthStencilView()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->ClearDepthStencilView(instance_->msaaDepthStencilView_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0));
    }

    void Graphics::ClearShadowMapDepthStencilView()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(instance_->context_->ClearDepthStencilView(instance_->shadowMapDepthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0));
    }

    void Graphics::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int baseVertexLocation)
    {
        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            instance_->annotation_->BeginEvent(L"DrawIndexed");
            instance_->context_->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
            instance_->annotation_->EndEvent();
        );
    }
    
    void Graphics::CheckAntiAliasingCapabilities()
    {
        antiAliasingCapabilities_.clear();

        for (uint32_t sampleCount = 1; ; sampleCount *= 2)
        {
            uint32_t qualityLevels;
            HRESULT result = device_->CheckMultisampleQualityLevels(GetBackBufferFormat(), sampleCount, &qualityLevels);

            if (FAILED(result) || qualityLevels == 0)
            {
                break;
            }
            else
            {
                AntiAliasingCapability cap
                {
                    .sampleCount = sampleCount,
                    .qualityLevels = qualityLevels
                };

                antiAliasingCapabilities_.push_back(cap);
            }
        }
    }

    int Graphics::GetWidth()
    {
        RECT rect;
        GetClientRect(instance_->hwnd_, &rect);
        return rect.right - rect.left;
    }

    int Graphics::GetHeight()
    {
        RECT rect;
        GetClientRect(instance_->hwnd_, &rect);
        return rect.bottom - rect.top;
    }

    void Graphics::CreateWindowSizeDependentResources()
    {
        RECT rect;
        GetClientRect(hwnd_, &rect);
        uint32_t width = rect.right - rect.left;
        uint32_t height = rect.bottom - rect.top;

        CreateSwapChain(width, height);
        CreateRenderTarget();
        CreateRenderTargetView();
        CreateDepthStencil(width, height);
        CreateDepthStencilView();
        CreateScreenViewport(width, height);
        CreateMSAARenderTarget(width, height);
        CreateMSAARenderTargetView();
        CreateMSAADepthStencilView(width, height);
        CreateShadowMapRenderTarget(4096, 4096);
        CreateShadowMapDepthStencilView();
        CreateShadowMapShaderResourceView();
        CreateShadowMapViewport(4096, 4096);
    }

    void Graphics::CreateSwapChain(uint32_t width, uint32_t height)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

        //set buffer dimensions and format
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = GetBackBufferFormat();
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = 0;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory_->CreateSwapChainForHwnd(
            device_.Get(),
            hwnd_,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            swapChain_.ReleaseAndGetAddressOf()
        ));
    }

    void Graphics::CreateRenderTarget()
    {
        DX::ThrowIfFailed(swapChain_->GetBuffer(0, IID_PPV_ARGS(renderTarget_.ReleaseAndGetAddressOf())));
    }

    void Graphics::CreateRenderTargetView()
    {
        CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, GetBackBufferFormat());

        DX::ThrowIfFailed(device_->CreateRenderTargetView(
            renderTarget_.Get(),
            &renderTargetViewDesc,
            renderTargetView_.ReleaseAndGetAddressOf()));
    }

    void Graphics::CreateDepthStencil(uint32_t width, uint32_t height)
    {
        // Create a depth stencil view for use with 3D rendering if needed.
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            GetDepthBufferFormat(),
            width,
            height,
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL);

        DX::ThrowIfFailed(device_->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            depthStencil_.ReleaseAndGetAddressOf()));
    }

    void Graphics::CreateDepthStencilView()
    {
        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
        DX::ThrowIfFailed(device_->CreateDepthStencilView(
            depthStencil_.Get(),
            &depthStencilViewDesc,
            depthStencilView_.ReleaseAndGetAddressOf()));
    }

    void Graphics::CreateScreenViewport(uint32_t width, uint32_t height)
    {
        // Set the 3D rendering viewport to target the entire window.
        screenViewport_ = CD3D11_VIEWPORT(
            0.0f,
            0.0f,
            static_cast<float>(width),
            static_cast<float>(height));
    }

    void Graphics::CreateMSAARenderTarget(uint32_t width, uint32_t height)
    {
        CD3D11_TEXTURE2D_DESC desc(
            GetBackBufferFormat(),
            width,
            height,
            1,
            1,
            D3D11_BIND_RENDER_TARGET,
            D3D11_USAGE_DEFAULT,
            0,
            settings_.antiAliasingSampleCount);

        DX::ThrowIfFailed(device_->CreateTexture2D(
            &desc,
            nullptr,
            msaaRenderTarget_.ReleaseAndGetAddressOf()));
    }

    void Graphics::CreateMSAARenderTargetView()
    {
        CD3D11_RENDER_TARGET_VIEW_DESC desc(D3D11_RTV_DIMENSION_TEXTURE2DMS, GetBackBufferFormat());

        DX::ThrowIfFailed(device_->CreateRenderTargetView(
            msaaRenderTarget_.Get(),
            &desc,
            msaaRenderTargetView_.ReleaseAndGetAddressOf()
        ));
    }

    void Graphics::CreateMSAADepthStencilView(uint32_t width, uint32_t height)
    {
        // Create an MSAA depth stencil view.
        CD3D11_TEXTURE2D_DESC desc(
            GetDepthBufferFormat(),
            width,
            height,
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL,
            D3D11_USAGE_DEFAULT,
            0,
            settings_.antiAliasingSampleCount
        );

        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
        DX::ThrowIfFailed(device_->CreateTexture2D(
            &desc,
            nullptr,
            depthStencil.GetAddressOf()
        ));

        DX::ThrowIfFailed(device_->CreateDepthStencilView(
            depthStencil.Get(),
            nullptr,
            msaaDepthStencilView_.ReleaseAndGetAddressOf()
        ));
    }

    void Graphics::CreateShadowMapRenderTarget(uint32_t width, uint32_t height)
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

        desc.Width = width;
        desc.Height = height;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.ArraySize = 1;
        desc.MipLevels = 1;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;

        DX::ThrowIfFailed(device_->CreateTexture2D(
            &desc,
            nullptr,
            shadowMapRenderTarget_.ReleaseAndGetAddressOf()));
    }

    void Graphics::CreateShadowMapDepthStencilView()
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

        desc.Format = DXGI_FORMAT_D32_FLOAT;
        desc.Texture2D.MipSlice = 0;
        desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        DX::ThrowIfFailed(device_->CreateDepthStencilView(
            shadowMapRenderTarget_.Get(), 
            &desc, 
            shadowMapDepthStencilView_.ReleaseAndGetAddressOf()));
    }

    void Graphics::CreateShadowMapShaderResourceView()
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

        desc.Format = DXGI_FORMAT_R32_FLOAT;
        desc.Texture2D.MipLevels = 1;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        DX::ThrowIfFailed(device_->CreateShaderResourceView(
            shadowMapRenderTarget_.Get(), 
            &desc, 
            shadowMapShaderResourceView_.ReleaseAndGetAddressOf()));
    }

    void Graphics::CreateShadowMapViewport(uint32_t width, uint32_t height)
    {
        ZeroMemory(&shadowMapViewport_, sizeof(D3D11_VIEWPORT));

        shadowMapViewport_.Width = static_cast<FLOAT>(width);
        shadowMapViewport_.Height = static_cast<FLOAT>(height);
        shadowMapViewport_.MinDepth = 0;
        shadowMapViewport_.MaxDepth = 1;
        shadowMapViewport_.TopLeftX = 0;
        shadowMapViewport_.TopLeftY = 0;
    }

    void Graphics::Present()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT
        (

            instance_->annotation_->BeginEvent(L"Present");

            {
                // The first argument instructs DXGI to block until VSync, putting the application
                // to sleep until the next VSync. This ensures we don't waste any cycles rendering
                // frames that will never be displayed to the screen.
                auto result = instance_->swapChain_->Present(1, 0);
                ENGINE_ASSERT(result == S_OK, "Couldn't present");
            }

            // Discard the contents of the render target.
            // This is a valid operation only when the existing contents will be entirely
            // overwritten. If dirty or scroll rects are used, this call should be removed.
            instance_->context_->DiscardView(instance_->renderTargetView_.Get());

            // Discard the contents of the depth stencil.
            instance_->context_->DiscardView(instance_->depthStencilView_.Get());

            instance_->annotation_->EndEvent();
        );
    }
}
