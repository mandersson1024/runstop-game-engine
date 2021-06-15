#include "ProjectGui.h"
#include "ImGui/imgui.h"
#include "Engine/Renderer/Graphics.h"
#include "Engine/Application/Application.h"
#include "spdlog/fmt/fmt.h"
#include "Engine/EngineAssert.h"
#include "EditorApplication.h"
#include "Engine/Assets/MaterialAsset.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Utils/StringUtils.h"
#include "Engine/ProjectSettings.h"
#include "Engine/Assets/Assets.h"

namespace Editor
{

    void ProjectGui::Draw()
    {
        static bool showDemoWindow = false;

        ImGui::Begin("Project");
        DrawScenes();
        DrawMaterials();
        DrawGraphics();
        ImGui::Checkbox("ImGui::ShowDemoWindow", &showDemoWindow);
        ImGui::End();

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow();
        }
    }

    void ProjectGui::DrawGraphics()
    {
        //ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode("Graphics Settings"))
        {
            Engine::GraphicsSettings settings = Engine::Graphics::GetSettings();

            ImGui::Indent();

            ImGui::Text("Anti Aliasing");
            ImGui::Indent();

            ImGui::Text(std::format("Sample Count: {}", settings.antiAliasingSampleCount).c_str());
            ImGui::SameLine();

            if (ImGui::Button("...###anti_aliasing_sample_count"))
            {
                ImGui::OpenPopup("anti_aliasing_sample_count_popup");
            }

            if (ImGui::BeginPopup("anti_aliasing_sample_count_popup"))
            {
                auto& caps = Engine::Graphics::GetAntiAliasingCapabilities();

                for (const auto& cap : Engine::Graphics::GetAntiAliasingCapabilities())
                {
                    if (ImGui::Selectable(std::format("{}", cap.sampleCount).c_str()))
                    {
                        settings.antiAliasingSampleCount = cap.sampleCount;
                        Engine::Graphics::ChangeSettings(settings);
                    }
                }

                ImGui::EndPopup();
            }

            {
                ImGui::Text(std::format("Quality: {}", settings.antiAliasingQuality).c_str());
                ImGui::SameLine();

                if (ImGui::Button("...###antialiasing_quality"))
                {
                    ImGui::OpenPopup("anti_aliasing_quality_popup");
                }

                if (ImGui::BeginPopup("anti_aliasing_quality_popup"))
                {
                    uint32_t currentSampleCount = Engine::Graphics::GetSettings().antiAliasingSampleCount;
                    uint32_t qualityLevels = 0;

                    for (const auto& cap : Engine::Graphics::GetAntiAliasingCapabilities())
                    {
                        if (cap.sampleCount == currentSampleCount)
                        {
                            qualityLevels = cap.qualityLevels;
                            break;
                        }
                    }

                    ENGINE_ASSERT(qualityLevels > 0, "No available graphics quality level found");

                    for (uint32_t i = 0; i < qualityLevels; ++i)
                    {
                        if (ImGui::Selectable(std::format("{}", i).c_str()))
                        {
                            settings.antiAliasingQuality = i;
                            Engine::Graphics::ChangeSettings(settings);
                        }
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::Unindent();
            ImGui::Unindent();

            ImGui::Separator();
            ImGui::TreePop();
        }
    }

    void ProjectGui::DrawScenes()
    {
        //ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode("Scenes"))
        {
            auto& projectSettings = Engine::Application::GetProjectSettings();
            const std::string& startupScene = projectSettings.GetStartupScene();

            for (auto& pair : Engine::Assets::GetAllSceneAssets())
            {
                auto sceneId = pair.first;

                bool isSelected = sceneId == startupScene;
                if (ImGui::Selectable(sceneId.c_str(), isSelected))
                {
                    if (sceneId != startupScene)
                    {
                        projectSettings.SetStartupScene(sceneId);
                        projectSettings.Save();

                        EditorApplication::Get().ApplyScene(sceneId);
                    }
                }
            }

            ImGui::Separator();
            ImGui::TreePop();
        }
    }

    void ProjectGui::DrawMaterialNotLoaded(const std::string& id)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, { 0.3f, 0.3f, 0.3f, 1 });

        if (ImGui::TreeNode(id.c_str())) 
        {
            ImGui::TreePop();
        };

        ImGui::PopStyleColor();
    }

    void ProjectGui::DrawMaterial(const std::string& id, std::shared_ptr<Engine::MaterialAsset> asset)
    {
        if (ImGui::TreeNode(id.c_str()))
        {
            auto material = asset->GetMaterial();

            ImGui::Separator();

            ImGui::Text(std::format("Shader: {}", material->shaderId).c_str());

            ImGui::Text(std::format("Blend State: {}", material->blendStateId).c_str());
            ImGui::Text(std::format("Rasterizer State: {}", material->rasterizerStateId).c_str());
            ImGui::Text(std::format("Sampler State: {}", material->samplerStateId).c_str());

            {
                ImGui::PushID("Basecolor");
                ImGui::Text("Basecolor");
                ImGui::Indent();

                ImGui::Text(std::format("Map: {}", material->basecolorMapId).c_str());

                ImGui::Text("Tint:");
                ImGui::SameLine();

                float basecolor[3]
                {
                    material->baseColorTint.x,
                    material->baseColorTint.y,
                    material->baseColorTint.z
                };

                if (ImGui::ColorEdit3("", basecolor, ImGuiColorEditFlags_NoInputs))
                {
                    material->baseColorTint = { basecolor[0], basecolor[1], basecolor[2] };
                }

                ImGui::Unindent();
                ImGui::PopID();
            }

            {
                ImGui::PushID("Roughness");
                ImGui::Text("Roughness");
                ImGui::Indent();

                ImGui::Text("Value:");
                ImGui::SameLine();
                ImGui::DragFloatRange2("", &material->roughnessRange.x, &material->roughnessRange.y, 0.001f, 0, 1, "%.3f");

                ImGui::Text(std::format("Map: {}", material->roughnessMapId).c_str());

                ImGui::Unindent();
                ImGui::PopID();
            }

            {
                ImGui::PushID("Metallic");
                ImGui::Text("Metallic");
                ImGui::Indent();

                ImGui::Text("Value:");
                ImGui::SameLine();
                ImGui::DragFloatRange2("", &material->metallicRange.x, &material->metallicRange.y, 0.001f, 0, 1, "%.3f");

                ImGui::Text(std::format("Map: {}", material->metallicMapId).c_str());

                ImGui::Unindent();
                ImGui::PopID();
            }

            {
                ImGui::PushID("Ambient Occlusion");
                ImGui::Text("Ambient Occlusion");
                ImGui::Indent();

                ImGui::Text("Value:");
                ImGui::SameLine();
                ImGui::DragFloatRange2("", &material->ambientOcclusionRange.x, &material->ambientOcclusionRange.y, 0.001f, 0, 1, "%.3f");

                ImGui::Text(std::format("Map: {}", material->ambientOcclusionMapId).c_str());

                ImGui::Unindent();
                ImGui::PopID();
            }

            {
                ImGui::PushID("UV Repeat");

                ImGui::Text("UV Repeat");
                ImGui::SameLine();
                ImGui::DragFloat("", &material->uvRepeat, 0.01f);

                ImGui::PopID();
            }

            if (ImGui::Button("Save"))
            {
                asset->Save();
            }

            ImGui::SameLine();
            if (ImGui::Button("Revert"))
            {
                asset->Load();
            }

            ImGui::Separator();
            ImGui::TreePop();
        }
    }

    void ProjectGui::DrawMaterials()
    {
        //ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode("Materials"))
        {
            for (const auto& pair : Engine::Assets::GetAllMaterialAssets())
            {
                std::string id = pair.first;
                auto asset = pair.second;

                if (id.starts_with("materials/"))
                {
                    id = Engine::RemovePrefix(id, "materials/");
                }

                if (asset->GetLoadingState() == Engine::AssetLoadingState::Loaded)
                {
                    DrawMaterial(id, asset);
                }
                else
                {
                    DrawMaterialNotLoaded(id);
                }
            }

            ImGui::Separator();
            ImGui::TreePop();
        }
    }

}
