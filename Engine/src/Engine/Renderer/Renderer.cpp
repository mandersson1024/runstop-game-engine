#include "pch.h"
#include <d3d11.h>
#include "Renderer.h"
#include "Graphics.h"
#include "ConstantBuffers\SceneConstantBufferVS.h"
#include "ConstantBuffers\SceneConstantBufferPS.h"
#include "ConstantBuffers\ModelConstantBufferVS.h"
#include "ConstantBuffers\ModelConstantBufferPS.h"
#include "SamplerState.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "LightSource.h"
#include "EnvironmentMap.h"
#include "Material.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "gpuprofiler\gpuprofiler.hpp"
#include "IndexBuffer.h"
#include "..\Scene\Camera.h"
#include "..\Scene\Transform.h"
#include "..\Assets\Assets.h"


namespace Engine
{
    Renderer::Renderer()
        : sceneConstantsVS_(std::make_unique<SceneConstantBufferVS>())
        , sceneConstantsPS_(std::make_unique<SceneConstantBufferPS>())
        , modelConstantsVS_(std::make_unique<ModelConstantBufferVS>())
        , modelConstantsPS_(std::make_unique<ModelConstantBufferPS>())
        , gpuProfiler_(std::make_unique<CGpuProfiler>())
        , environment_(nullptr)
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(
            bool result = gpuProfiler_->Init(Graphics::GetDevice(), Graphics::GetContext());
            ENGINE_ASSERT(result, "");
        );
    }

    Renderer::~Renderer()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->Shutdown());
    }

    int Renderer::GetNumDrawCallsLastFrame() const
    {
        return numDrawCallsLastFrame_;
    }

    int Renderer::GetTriangleCountLastFrame() const
    {
        return triangleCountLastFrame_;
    }

    void Renderer::SubmitMesh(
        std::shared_ptr<Mesh> mesh,
        std::string materialId,
        DirectX::XMMATRIX modelMatrix,
        bool castsShadow, 
        bool debugInvertColors)
    {
        submits_.emplace_back(mesh, materialId, modelMatrix, castsShadow, debugInvertColors);
    }

    void Renderer::SetEnvironment(
        const std::string& id,
        std::shared_ptr<Mesh> mesh,
        std::string materialId,
        DirectX::XMMATRIX modelMatrix,
        float mipMapLevel)
    {
        Environment environment
        {
            .id = id,
            .mesh = mesh,
            .materialId = materialId,
            .modelMatrix = modelMatrix,
            .mipMapLevel = mipMapLevel,
        };

        environment_ = std::make_unique<Environment>(environment);

        sceneConstantsPS_->SetDebugSkyboxMipMapLevel(environment_->mipMapLevel);
        Assets::GetSamplerState("skybox")->Bind(2);
    }

    void Renderer::ClearEnvironment()
    {
        environment_.reset();

        Assets::GetSamplerState("skybox")->Bind(2); // TODO: Hack to suppress warnings in pixel shader when no skybox is set
        Assets::GetSamplerState("brdf")->Bind(3); // TODO: Hack to suppress warnings in pixel shader when no skybox is set
    }

    void Renderer::Clear()
    {
        submits_.clear();
        numDrawCallsCurrentFrame_ = 0;
        triangleCountCurrentFrame_ = 0;
    }

    void Renderer::SetCamera(std::shared_ptr<Camera> camera)
    {
        state_.camera = camera;
    }

    void Renderer::SetLightSource(std::shared_ptr<LightSource> light)
    {
        state_.light = light;
    }

    void Renderer::BeginScene()
    {
        if (state_.camera)
        {
            sceneConstantsVS_->SetCameraViewProjectionMatrix(state_.camera->GetViewProjectionMatrix());
            sceneConstantsPS_->SetCameraPosition(state_.camera->GetPosition());
        }

        if (state_.light)
        {
            sceneConstantsVS_->SetLightViewProjectionMatrix(state_.light->GetTransform()->GetCamera()->GetViewProjectionMatrix());
            sceneConstantsVS_->SetLightPosition(state_.light->GetTransform()->GetPosition());
            sceneConstantsPS_->SetLightPosition(state_.light->GetTransform()->GetPosition());
            sceneConstantsPS_->SetLightDirection(state_.light->GetTransform()->GetForward());
            sceneConstantsPS_->SetLightColor(state_.light->GetColor());
        }

        sceneConstantsVS_->Bind(0);
        sceneConstantsPS_->Bind(0);
    }

    void Renderer::RenderScene()
    {
        WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->BeginFrame());

        Graphics::ClearMSAARenderTargetView(state_.clearColor);

        //if (false)
        {
            //
            // SHADOW MAP PASS
            //
            
            // TODO:
            // Only one shader here. We should easily be able
            // to reduce this to only one draw call.

            Graphics::ClearShadowMapDepthStencilView();
            Graphics::SetShadowMapRenderTarget();
            Graphics::SetShadowMapViewport();

            BindMaterial(Assets::GetShadowMapMaterial());
            Assets::GetSamplerState("shadow_map")->Bind(1);

            WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->Timestamp(GTS_ShadowPrepare));

            for (auto& submission : submits_)
            {
                if (submission.castsShadow)
                {
                    modelConstantsVS_->SetModelMatrix(submission.modelMatrix);
                    modelConstantsVS_->Bind(1);
                    submission.mesh->Bind();

                    DrawIndexed(submission.mesh->GetIndexBuffer()->GetCount());
                }
            }

            WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->Timestamp(GTS_ShadowDraw));
        }

        //if (false)
        if (environment_)
        {
            //
            // SKYBOX PASS
            //
            // TODO: For now, we just fill the screen with the skybox
            // TODO: Later on we should do this pass after the main pass
            // TODO: and only paint what has been left blank in previous passes.
            //

            Graphics::SetMSAARenderTarget();
            Graphics::SetScreenViewport();

            modelConstantsVS_->SetModelMatrix(environment_->modelMatrix);
            modelConstantsVS_->Bind(1);

            BindMaterial(Assets::GetMaterial(environment_->materialId));
            
            if (debugDrawSkyboxAsRadianceMap_)
            {
                Assets::GetRadianceMap(environment_->id)->Bind(2);
            }
            else if (debugDrawSkyboxAsIrradianceMap_)
            {
                Assets::GetIrradianceMap(environment_->id)->Bind(2);
            }
            else
            {
                Assets::GetSkybox(environment_->id)->Bind(2);
            }

            Assets::GetIrradianceMap(environment_->id)->Bind(3);
            Assets::GetRadianceMap(environment_->id)->Bind(4);

            Assets::GetBRDFTexture()->Bind(5);
            Assets::GetSamplerState("brdf")->Bind(3);

            environment_->mesh->Bind();
            DrawIndexed(environment_->mesh->GetIndexBuffer()->GetCount());
            triangleCountCurrentFrame_ += environment_->mesh->GetTriangleCount();
        }

        //if (false)
        {
            //
            // MAIN PASS
            //

            Graphics::ClearMSAADepthStencilView();
            Graphics::SetMSAARenderTarget();
            Graphics::SetShadowMapShaderResource();
            Graphics::SetScreenViewport();

            WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->Timestamp(GTS_MainPrepare));

            for (auto& submission : submits_)
            {
                auto material = Assets::GetMaterial(submission.materialId);

                modelConstantsVS_->SetModelMatrix(submission.modelMatrix);
                modelConstantsVS_->SetUVRepeat(material->uvRepeat);
                modelConstantsVS_->Bind(1);

                modelConstantsPS_->SetBaseColorTint(material->baseColorTint);
                modelConstantsPS_->SetRoughnessRange(material->roughnessRange);
                modelConstantsPS_->SetMetallicRange(material->metallicRange);
                modelConstantsPS_->SetAmbientOcclusionRange(material->ambientOcclusionRange);
                modelConstantsPS_->SetDebugInvertColors(submission.debugInvertColors);
                modelConstantsPS_->Bind(1);

                BindMaterial(material);

                submission.mesh->Bind();
                DrawIndexed(submission.mesh->GetIndexBuffer()->GetCount());
                triangleCountCurrentFrame_ += submission.mesh->GetTriangleCount();
            }

            Graphics::ReleaseShadowMapShaderResource();
        }

        WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->Timestamp(GTS_MainDraw));

        {
            //
            // POST PROCESS PASS
            //

            WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->Timestamp(GTS_PostPrepare));
            WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->Timestamp(GTS_PostDraw));
        }
    }

    void Renderer::BeginGizmos()
    {
        Graphics::ClearMSAADepthStencilView();
    }

    void Renderer::RenderGizmo(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, DirectX::XMMATRIX modelMatrix)
    {
        Graphics::SetMSAARenderTarget();
        Graphics::SetScreenViewport();

        //for (auto& submission : m_submits)
        {
            modelConstantsVS_->SetModelMatrix(modelMatrix);
            modelConstantsVS_->Bind(1);

            modelConstantsPS_->SetBaseColorTint(material->baseColorTint);
            modelConstantsPS_->Bind(1);

            BindMaterial(material);

            mesh->Bind();
            DrawIndexed(mesh->GetIndexBuffer()->GetCount());
        }
    }

    void Renderer::Present()
    {
        Graphics::ResolveMSAARenderTarget();

        WITH_GUARDED_GRAPHICS_CONTEXT(gpuProfiler_->WaitForDataAndUpdate());

        Graphics::Present();
        
        WITH_GUARDED_GRAPHICS_CONTEXT
        (
            gpuProfiler_->EndFrame();

            float dTDrawTotal = 0.0f;
            for (GTS gts = GTS_BeginFrame; gts < GTS_EndFrame; gts = GTS(gts + 1))
                dTDrawTotal += gpuProfiler_->DtAvg(gts);

            gpuProfileData_.totalGPUTime = 1000 * dTDrawTotal;
            gpuProfileData_.shadowPrepare = 1000 * gpuProfiler_->DtAvg(GTS_ShadowPrepare);
            gpuProfileData_.shadowDraw = 1000 * gpuProfiler_->DtAvg(GTS_ShadowDraw);
            gpuProfileData_.mainPrepare = 1000 * gpuProfiler_->DtAvg(GTS_MainPrepare);
            gpuProfileData_.mainDraw = 1000 * gpuProfiler_->DtAvg(GTS_MainDraw);
            gpuProfileData_.postPrepare = 1000 * gpuProfiler_->DtAvg(GTS_PostPrepare);
            gpuProfileData_.postDraw = 1000 * gpuProfiler_->DtAvg(GTS_PostDraw);
            gpuProfileData_.frameTime = 1000 * (dTDrawTotal + gpuProfiler_->DtAvg(GTS_EndFrame));
        );

        numDrawCallsLastFrame_ = numDrawCallsCurrentFrame_;
        triangleCountLastFrame_ = triangleCountCurrentFrame_;
    }

    void Renderer::DrawIndexed(uint32_t indexCount)
    {
        Graphics::DrawIndexed(indexCount, 0, 0);
        numDrawCallsCurrentFrame_++;
    }

    void Renderer::SetClearColor(float color[4])
    {
        SetClearColor({ color[0], color[1], color[2], color[3] });
    }

    void Renderer::SetClearColor(DirectX::XMVECTOR color)
    {
        state_.clearColor = color;
    }

    void Renderer::BindMaterial(std::shared_ptr<Material> material)
    {
        Assets::GetShader(material->shaderId)->Bind();
        Assets::GetBlendState(material->blendStateId)->Bind();
        Assets::GetRasterizerState(material->rasterizerStateId)->Bind();
        Assets::GetSamplerState(material->samplerStateId)->Bind(0);
        Assets::GetBasecolorMapTexture(material->basecolorMapId)->Bind(0);
        Assets::GetNormalMapTexture(material->normalMapId)->Bind(6);
        Assets::GetRoughnessMapTexture(material->roughnessMapId)->Bind(7);
        Assets::GetMetallicMapTexture(material->metallicMapId)->Bind(8);
        Assets::GetAmbientOcclusionMapTexture(material->ambientOcclusionMapId)->Bind(9);

        if (debugIgnoreAmbientOcclusion_)
        {
            Assets::GetDefaultTextureWhite()->Bind(9);
        }
    }

    void Renderer::SetDebug(
        bool ignoreLightSources, 
        bool ignoreAmbientIBL, 
        bool ignoreSpecularIBL, 
        bool ignoreAmbientOcclusion,
        bool debugDrawSkyboxAsRradianceMap,
        bool debugDrawSkyboxAsIrradianceMap)
    {
        sceneConstantsPS_->SetDebugIgnoreLightSources(ignoreLightSources);
        sceneConstantsPS_->SetDebugIgnoreAmbientIBL(ignoreAmbientIBL);
        sceneConstantsPS_->SetDebugIgnoreSpecularIBL(ignoreSpecularIBL);
        debugIgnoreAmbientOcclusion_ = ignoreAmbientOcclusion;
        debugDrawSkyboxAsRadianceMap_ = debugDrawSkyboxAsRradianceMap;
        debugDrawSkyboxAsIrradianceMap_ = debugDrawSkyboxAsIrradianceMap;
    }

}

