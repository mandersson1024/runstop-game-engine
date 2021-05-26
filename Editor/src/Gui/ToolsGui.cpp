#include "ToolsGui.h"
#include "ImGui/imgui.h"
#include "EditorApplication.h"
#include "Engine/Application/Application.h"
#include "Engine/Renderer/Graphics.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/ProjectSettings.h"
#include "Engine/Renderer/TextureCube.h"
#include "Engine/Assets/Assets.h"

namespace Editor
{

    void ToolsGui::Draw()
    {
        ImGui::Begin("Tools");
        {
            const auto& data = Engine::Application::GetRenderer().GetGpuProfileData();
            ImGui::Text("Total GPU time: %.2f ms (%.1f FPS)", data.totalGPUTime, 1000.0f / data.totalGPUTime);
            ImGui::Indent();
            ImGui::Text("Shadow (prepare): %.2f ms", data.shadowPrepare);
            ImGui::Text("Shadow (draw): %.2f ms", data.shadowDraw);
            ImGui::Text("Main (prepare): %.2f ms", data.mainPrepare);
            ImGui::Text("Main (draw): %.2f ms", data.mainDraw);
            ImGui::Text("Post (prepare): %.2f ms", data.postPrepare);
            ImGui::Text("Post (draw): %.2f ms", data.postDraw);
            ImGui::Unindent();
            ImGui::Text("Frame time: %.2f ms (%.1f FPS)", data.frameTime, 1000.0f / data.frameTime);
            ImGui::Separator();
        }

        ImGui::Text("Triangle count: %d", Engine::Application::GetRenderer().GetTriangleCountLastFrame());
        ImGui::Text("Draw calls: %d", Engine::Application::GetRenderer().GetNumDrawCallsLastFrame());
        ImGui::Text("%.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Separator();

        {
            auto clearColor = Engine::Application::GetRenderer().GetClearColor();

            if (ImGui::ColorEdit4("Clear Color", (float*)&clearColor, ImGuiColorEditFlags_NoInputs))
            {
                Engine::Application::GetRenderer().SetClearColor(clearColor);
            }
        }

        {
            bool b = Engine::Application::GetProjectSettings().GetDrawHelperLines();
            if (ImGui::Checkbox("Draw Helper Lines", &b))
            {
                Engine::Application::GetProjectSettings().SetDrawHelperLines(b);
            }
        }

        {
            bool b = Engine::Application::GetProjectSettings().GetDrawColliders();
            if (ImGui::Checkbox("Draw Colliders", &b))
            {
                Engine::Application::GetProjectSettings().SetDrawColliders(b);
            }
        }

        auto scene = EditorApplication::Get().GetActiveScene();

        if (scene->HasEnvironmentMap())
        {
            bool pointSampler = Engine::Application::GetProjectSettings().GetDrawSkyboxUsingPointSampler();

            if (ImGui::Checkbox("Draw Skybox using Point Sampler", &pointSampler))
            {
                EditorApplication::GetProjectSettings().SetDrawSkyboxUsingPointSampler(pointSampler);
            }
            
            ImGui::PushItemWidth(-140);

            int type = static_cast<int>(Engine::Application::GetProjectSettings().GetSkyboxDisplayType());
            const char* items[] = { "Skybox", "Irradiance Map", "Radiance Map" };
            if (ImGui::Combo("Display as Skybox", &type, items, IM_ARRAYSIZE(items)))
            {
                Engine::Application::GetProjectSettings().SetSkyboxDisplayType(type);
            }

            if (static_cast<SkyboxDisplayType>(Engine::Application::GetProjectSettings().GetSkyboxDisplayType()) == SkyboxDisplayType::Radiance)
            {
                const int maxRoughnessLevel = Engine::Assets::GetRadianceMap(scene->GetEnvironmentMapId())->GetNumMipLevels() - 1;
                float roughness = Engine::Application::GetProjectSettings().GetRadianceMapRoughness();

                if (ImGui::SliderFloat("Roughness", &roughness, 0, static_cast<float>(maxRoughnessLevel)))
                {
                    EditorApplication::GetProjectSettings().SetRadianceMapRoughness(roughness);
                }
            }

            {
                bool b = EditorApplication::GetProjectSettings().GetIgnoreLightSources();
                if (ImGui::Checkbox("Ignore Light Sources", &b))
                {
                    EditorApplication::GetProjectSettings().SetIgnoreLightSources(b);
                }
            }

            {
                bool b = EditorApplication::GetProjectSettings().GetIgnoreDiffuseIBL();
                if (ImGui::Checkbox("Ignore Diffuse IBL", &b))
                {
                    EditorApplication::GetProjectSettings().SetIgnoreDiffuseIBL(b);
                }
            }

            {
                bool b = EditorApplication::GetProjectSettings().GetIgnoreSpecularIBL();
                if (ImGui::Checkbox("Ignore Specular IBL", &b))
                {
                    EditorApplication::GetProjectSettings().SetIgnoreSpecularIBL(b);
                }
            }

            {
                bool b = EditorApplication::GetProjectSettings().GetIgnoreAmbientOcclusion();
                if (ImGui::Checkbox("Ignore Ambient Occlusion", &b))
                {
                    EditorApplication::GetProjectSettings().SetIgnoreAmbientOcclusion(b);
                }
            }

            ImGui::PopItemWidth();
        }

        if (ImGui::TreeNode("Shadow Map"))
        {
            ImTextureID id = static_cast<void*>(Engine::Graphics::GetShadowMapShaderResourceView());
            ImGui::Image(id, { 256, 256 });
            ImGui::TreePop();
        }

        ImGui::End();
    }
}
