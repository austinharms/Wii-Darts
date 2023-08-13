#ifndef DARTS_RENDERER_CORE_H_
#define DARTS_RENDERER_CORE_H_
#include "Core.h"
#include "TextureHandle.h"
#include <ogc/gx_struct.h>
#include <ogc/gu.h>

namespace darts {
	class EngineCore;

	class Renderer
	{
	public:
		~Renderer();
		WD_NOCOPY(Renderer);
		// Loads the framebuffer size into width and height
		// width or height may be null
		void GetFramebufferSize(uint16_t* width, uint16_t* height) const;

		void SetCameraTransform(Mtx transform);

		void SetFOV(float fov);

		void SetClippingPlanes(float near, float far);

		void SetClearColor(uint32_t color);

		GXRModeObj& GetVideoMode();

	private:		
		friend class EngineCore;

		GXRModeObj* m_videoMode;
		void* m_videoFIFO;
		void* m_framebuffers[2];
		float m_fov;
		float m_nearPlane;
		float m_farPlane;
		TextureHandle m_defaultTexture;
		uint8_t m_activeFramebuffer;
		bool m_enabled;

		Renderer();

		// Video Mode Correction Taken from: https://github.com/GRRLIB/GRRLIB/blob/master/GRRLIB/GRRLIB/GRRLIB_core.c
		void CorrectVideoMode();

		void WaitVSync() const;

		void SetupGX();

		void Enabled();

		void Disable();

		void SwapBuffers();

		void UpdateProjectionMatrix();

		void ResetViewMatrix();

		void DrawTestRect();
	};
}
#endif // !DARTS_ENGINE_CORE_H_
