/*
#include <d3d11.h>
#include "gpuprofiler.hpp"

// Pseudocode example of using the GPU profiler

ID3D11Device * g_pDevice = NULL;
ID3D11DeviceContext * g_pContext = NULL;
IDXGISwapChain * g_pSwapChain = NULL;

bool Init ()
{
	// Do other initialization...

	if (!g_gpuProfiler.Init())
		return false;

	return true;
}

void Shutdown ()
{
	// Do other shutdown...

	g_gpuProfiler.Shutdown();
}

void ClearShadowMap ();
void DrawStuffInShadowMap ();
void ClearMainView ();
void DrawStuffInMainView ();
void ScreenPrintf (const char * message, ...);

void Render ()
{
	g_gpuProfiler.BeginFrame();
 
    // Draw shadow map

	ClearShadowMap();
	g_gpuProfiler.Timestamp(GTS_ShadowClear);

	DrawStuffInShadowMap();
	g_gpuProfiler.Timestamp(GTS_ShadowObjects);
 
    // Draw main view
 
	ClearMainView();
	g_gpuProfiler.Timestamp(GTS_MainClear);

	DrawStuffInMainView();
	g_gpuProfiler.Timestamp(GTS_MainObjects);

	// Draw performance readout - at end of CPU frame, so hopefully the previous frame
	//  (whose data we're getting) will have finished on the GPU by now.

	g_gpuProfiler.WaitForDataAndUpdate();

	float dTDrawTotal = 0.0f;
	for (GTS gts = GTS_BeginFrame; gts < GTS_EndFrame; gts = GTS(gts + 1))
		dTDrawTotal += g_gpuProfiler.DtAvg(gts);

	ScreenPrintf(
		"Draw time: %0.2f ms\n"
		"   Shadow clear: %0.2f ms\n"
		"   Shadow objects: %0.2f ms\n"
		"   Main clear: %0.2f ms\n"
		"   Main objects: %0.2f ms\n"
		"GPU frame time: %0.2f ms\n",
		1000.0f * dTDrawTotal,
		1000.0f * g_gpuProfiler.DtAvg(GTS_ShadowClear),
		1000.0f * g_gpuProfiler.DtAvg(GTS_ShadowObjects),
		1000.0f * g_gpuProfiler.DtAvg(GTS_MainClear),
		1000.0f * g_gpuProfiler.DtAvg(GTS_MainObjects),
		1000.0f * (dTDrawTotal + g_gpuProfiler.DtAvg(GTS_EndFrame)));
 
    // Display frame on-screen and finish up queries

	g_pSwapChain->Present(1, 0);
	g_gpuProfiler.EndFrame();
}

*/

