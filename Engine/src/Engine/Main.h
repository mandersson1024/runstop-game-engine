
#pragma once
#include <memory>
#include "Logger.h"
#include "EngineAssert.h"
#include "Application/Application.h"
#include "Platform/ConsoleWindow.h"

// Indicates to hybrid graphics systems to prefer the discrete part by default
// See also: https://stackoverflow.com/questions/55435230/how-to-ensure-directx-11-app-use-the-discrete-gpu-on-a-dual-gpu-laptop-with-c
extern "C"
{
        __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
        __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace Engine
{
    extern std::shared_ptr<Application> CreateApplication();
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    {
        bool result = DirectX::XMVerifyCPUSupport();
        ENGINE_ASSERT(result, "DirectXMath is not supported");
    }

    {
        auto result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
        ENGINE_ASSERT(!FAILED(result), "COM library couldn't be initialized");
    }

    if (IsDebuggerPresent())
    {
        Engine::ConsoleWindow::Create();
    }

    Engine::Logger::Init();

    {
        auto app = Engine::CreateApplication();
        app->Init(hInstance, nCmdShow);
        app->Run();
        app->Shutdown();
    }

    Engine::Logger::Shutdown();

    if (IsDebuggerPresent())
    {
        Engine::ConsoleWindow::Destroy();
    }

    CoUninitialize();
}
