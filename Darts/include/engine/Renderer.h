#ifndef DARTS_RENDERER_CORE_H_
#define DARTS_RENDERER_CORE_H_
#include "Core.h"
#include "TextureHandle.h"
#include "Transform.h"
#include <ogc/gx_struct.h>
#include <ogc/gu.h>

class Engine;
class RenderMesh;

enum WdRenderModeEnum
{
	WD_RENDER_IDENT = 0,
	WD_RENDER_VIEW = 3,
	WD_RENDER_STACK = 6,
	WD_RENDER_UI = 9
};

class Renderer
{
public:
	~Renderer();
	WD_NOCOPY(Renderer);

	GXRModeObj& GetVideoMode();
	// Loads the framebuffer size into width and height
	// width or height may be null
	void GetFramebufferSize(uint16_t* width, uint16_t* height) const;
	void SetCameraTransform(const Transform& t);
	void SetFOV(float fov);
	void SetClippingPlanes(float near, float far);
	void SetClearColor(uint32_t color);
	void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	void ResetScissor();
	void PushTransform(const Transform& t);
	void PushIdentityTransform();
	void PopTransform();
	void DrawRenderMesh(const RenderMesh& mesh, TextureHandle* texture = nullptr, WdRenderModeEnum mode = WD_RENDER_STACK);

private:
	friend class Engine;

	GXRModeObj* m_videoMode;
	void* m_videoFIFO;
	void* m_framebuffers[2];
	float m_fov;
	float m_nearPlane;
	float m_farPlane;
	TextureHandle m_defaultTexture;
	Transform m_viewTransform;
	Transform m_transformStack[128];
	Mtx44 m_perspecProjection;
	Mtx44 m_orthoProjection;
	WdRenderModeEnum m_currentRenderMode;
	uint8_t m_transformStackIndex;
	uint8_t m_activeFramebuffer;
	bool m_enabled;
	bool m_frameStarted;

	// ##### Methods called by Engine #####

	Renderer();
	void Enable();
	void Disable();
	void StartFrame();
	void EndFrame();

	// ##### Internal use methods #####

	void SwapBuffers();
	void WaitVSync() const;
	void SetupVideo();
	// Video Mode Correction Taken from: https://github.com/GRRLIB/GRRLIB/blob/master/GRRLIB/GRRLIB/GRRLIB_core.c
	void CorrectVideoMode();
	void SetupGX();
	void SetupTEV();
	void SetupVtxAttribs();
	void SetupMatrices();
	void UpdatePerspectiveProjectionMatrix();
	void UpdateActiveMatrix(WdRenderModeEnum mode);
	void DrawIndexedMesh(const RenderMesh& mesh);
	void DrawNonIndexedMesh(const RenderMesh& mesh);

};
#endif // !DARTS_ENGINE_CORE_H_
