#include "pch.h"
#include "ImGuiLayer.h"
#include "Engine/Platform/Window.h"
#include "ImGui\imgui.h"
#include "Engine\Renderer\Graphics.h"
#include "Engine\Application\Application.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\imgui_impl_win32.h"
#include "EditorApplication.h"
#include "../Gui/ToolsGui.h"
#include "../Gui/ProjectGui.h"
#include "../Gui/SceneGui.h"
#include "Engine\Platform\NativeWindow.h"

using namespace Engine;

namespace Editor
{

    ImGuiLayer::ImGuiLayer(Engine::Window& window)
        : Layer("ImGui layer")
        , window_(window)
        , toolsGui_(std::make_unique<ToolsGui>())
        , projectGui_(std::make_unique<ProjectGui>())
        , sceneGui_(std::make_unique<SceneGui>())
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup Dear ImGui context
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;
        //io.ConfigViewportsNoDefaultParent = true;
        //io.ConfigDockingAlwaysTabBar = true;
        //io.ConfigDockingTransparentPayload = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer bindings
        ImGui_ImplWin32_Init(window_.GetNativeWindow().GetHWnd());

        WITH_GUARDED_GRAPHICS_CONTEXT(
            ImGui_ImplDX11_Init(
                Engine::Graphics::GetDevice(),
                Engine::Graphics::GetContext());
        );
    }

	ImGuiLayer::~ImGuiLayer()
	{
        window_.GetNativeWindow().RemoveWinProcHandler();
        WITH_GUARDED_GRAPHICS_CONTEXT(ImGui_ImplDX11_Shutdown());
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& evt)
	{
		if (evt.GetEventType() == EventType::WindowResize)
		{

		}

		ImGuiIO& io = ImGui::GetIO();
		evt.handled |= evt.IsMouseEvent() & io.WantCaptureMouse;
		evt.handled |= evt.IsKeyEvent() & io.WantCaptureKeyboard;
	}

	void ImGuiLayer::Begin()
	{
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
        ImGui::Render();
        WITH_GUARDED_GRAPHICS_CONTEXT(ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()));

        ImGuiIO & io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            WITH_GUARDED_GRAPHICS_CONTEXT(ImGui::RenderPlatformWindowsDefault());
        }
	}

    void ImGuiLayer::OnAttach()
    {
        window_.GetNativeWindow().SetWinProcHandler(
            [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
            {
                extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
                return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
            });
    }

    void ImGuiLayer::OnDetach()
    {
        window_.GetNativeWindow().RemoveWinProcHandler();
    }

    void ImGuiLayer::OnUpdate(float deltaTime)
    {
        Begin();

        toolsGui_->Draw();
        projectGui_->Draw();
        sceneGui_->Draw();

        End();
    }

}
