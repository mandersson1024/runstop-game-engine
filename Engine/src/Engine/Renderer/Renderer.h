#pragma once
#include <string>
#include <memory>
#include <vector>
#include <DirectXMath.h>

class CGpuProfiler;

namespace Engine
{
    class Mesh;
	class Camera;
	class LightSource;
	class SceneConstantBufferVS;
	class SceneConstantBufferPS;
	class ModelConstantBufferVS;
	class ModelConstantBufferPS;
	struct Material;

	struct GpuProfileData
	{
		float totalGPUTime;
        float shadowPrepare;
		float shadowDraw;
		float mainPrepare;
		float mainDraw;
		float postPrepare;
		float postDraw;
        float frameTime;
	};

	class Renderer
	{
	private:
        struct Environment
        {
            std::string id;
            std::shared_ptr<Mesh> mesh;
            std::string materialId;
            DirectX::XMMATRIX modelMatrix;
			float mipMapLevel;
        };

		struct Submission
		{
			std::shared_ptr<Mesh> mesh;
            std::string materialId;
			DirectX::XMMATRIX modelMatrix;
			bool castsShadow;
			bool debugInvertColors;
		};

	public:
		Renderer();
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator= (const Renderer&) = delete;

		void Clear();
		void SetLightSource(std::shared_ptr<LightSource> light);
		void BeginScene();
		void RenderScene();
		void BeginGizmos();
		void RenderGizmo(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, DirectX::XMMATRIX modelMatrix);
		void Present();

        std::shared_ptr<Camera> GetCamera() const { return state_.camera; }
        void SetCamera(std::shared_ptr<Camera> camera);

		DirectX::XMVECTOR GetClearColor() { return state_.clearColor; }
		void SetClearColor(DirectX::XMVECTOR color);
		void SetClearColor(float color[4]);

		void SubmitMesh(
			std::shared_ptr<Mesh> mesh,
			std::string materialId,
			DirectX::XMMATRIX modelMatrix,
			bool castsShadow, 
			bool debugInvertColors);

		void SetEnvironment(
            const std::string& id,
            std::shared_ptr<Mesh> mesh,
            std::string materialId,
            DirectX::XMMATRIX modelMatrix,
			float mipMapLevel);

		void ClearEnvironment();

		int GetNumDrawCallsLastFrame() const;
		int GetTriangleCountLastFrame() const;
		const GpuProfileData& GetGpuProfileData() const { return gpuProfileData_; }

        void BindMaterial(std::shared_ptr<Material> material);

		void SetDebug(
			bool ignoreLightSources,
			bool ignoreAmbientIBL,
			bool ignoreSpecularIBL,
			bool ignoreAmbientOcclusion,
			bool debugDrawSkyboxAsRradianceMap,
			bool debugDrawSkyboxAsIrradianceMap);
	private:
		void DrawIndexed(uint32_t indexCount);

	private:
		struct RenderState
		{
			RenderState()
				: clearColor({ 0, 0.435f, 0.569f, 1 })
			{}

			DirectX::XMVECTOR clearColor;
			std::shared_ptr<Camera> camera;
			std::shared_ptr<LightSource> light;
		};

	private:
        std::unique_ptr<CGpuProfiler> gpuProfiler_;
		GpuProfileData gpuProfileData_;
		RenderState state_;
        std::unique_ptr<SceneConstantBufferVS> sceneConstantsVS_;
        std::unique_ptr<SceneConstantBufferPS> sceneConstantsPS_;
        std::unique_ptr<ModelConstantBufferVS> modelConstantsVS_;
        std::unique_ptr<ModelConstantBufferPS> modelConstantsPS_;
		std::vector<Submission> submits_;

        std::unique_ptr<Environment> environment_;

        int numDrawCallsCurrentFrame_ = 0;
        int numDrawCallsLastFrame_ = 0;

		int triangleCountCurrentFrame_ = 0;
        int triangleCountLastFrame_ = 0;

		bool debugIgnoreAmbientOcclusion_    = false;
		bool debugDrawSkyboxAsRadianceMap_   = false;
		bool debugDrawSkyboxAsIrradianceMap_ = false;
	};

}

