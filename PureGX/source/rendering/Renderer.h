#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdint.h>
#include <malloc.h>
#include <ogc/conf.h>
#include <ogc/machine/processor.h>
#include <gctypes.h>

#include "Texture.h"
#include "Mesh.h"
#include "../Vector3f.h"
#include "RenderMesh3D.h"
#include "font_png.h"

#define DEFAULT_FIFO_SIZE (256 * 1024)

class Renderer
{
public:
	static Renderer& getInstance()
	{
		static Renderer instance;
		return instance;
	}

	void drawRenderMesh(RenderMesh3D* renderMesh, Vector3f position, uint32_t color = 0xffffffff) {
		//GRRLIB_ObjectViewBegin();
		//GRRLIB_ObjectViewTrans(position.x, position.y, position.z);
		//GRRLIB_ObjectViewEnd();
		drawMesh(renderMesh, color);
	}

	void drawRenderMesh(RenderMesh3D* renderMesh, uint32_t color = 0xffffffff) {
		//GRRLIB_ObjectViewBegin();
		//GRRLIB_ObjectViewEnd();
		drawMesh(renderMesh, color);
	}

	void swapFrameBuffer();

	void drawText(uint32_t x, uint32_t y, const char* text, float size = 1, uint32_t color = 0x000000FF) {
		//GRRLIB_2dMode();
		//GRRLIB_SetTexture(font_texture, 0);
		//GRRLIB_Printf(x, y, font_texture, color, size, text);
		//GRRLIB_PrintfTTF(x, y, font, text, size, color);
	}

	const GXRModeObj* getVideoMode() const {
		return _vmode;
	}

private:
	GXRModeObj* _vmode;
	void* _frameBuffers[2];
	void* _fifoBuffer;
	Mtx _view;
	uint8_t _activeFramebuffer;

	Renderer();

	~Renderer();

	void drawMesh(RenderMesh3D* renderMesh, const uint32_t color) {
		renderMesh->getTexture()->bind();
		DrawTestRect();
		//uint32_t vertCount = renderMesh->getMesh()->getVertCount();
		//const float* verts = renderMesh->getMesh()->getVertexBuffer();
		//uint32_t bufferIndex = 0;
		//GRRLIB_3dMode(0.1, 100, 60, true, false);
		//GX_Begin(GX_TRIANGLES, GX_VTXFMT0, vertCount);
		//for (uint32_t i = 0; i < vertCount; ++i) {
		//	GX_Position3f32(verts[bufferIndex++], verts[bufferIndex++], verts[bufferIndex++]);
		//	GX_Color1u32(color);
		//	GX_TexCoord2f32(verts[bufferIndex++], verts[bufferIndex++]);
		//}

		//GX_End();
	}

	void DrawTestRect() const;

	Renderer(Renderer const&);
	void operator=(Renderer const&);
};
#endif // !RENDERER_H_
