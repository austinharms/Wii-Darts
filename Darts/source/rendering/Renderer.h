#ifndef RENDERER_H_
#define RENDERER_H_
#include <grrlib.h>
#include <stdint.h>
#include <malloc.h>

#include "Mesh.h"
#include "Texture.h"
#include "../Vector3f.h"
#include "RenderMesh3D.h"
#include "font_png.h"

class Renderer
{
public:
	static Renderer& getInstance()
	{
		static Renderer instance;
		return instance;
	}

	void drawRenderMesh(const RenderMesh3D* renderMesh, Vector3f position, uint32_t color = 0xffffffff) const {
		GRRLIB_ObjectViewBegin();
		//GRRLIB_ObjectViewTrans(position.x, position.y, position.z);
		GRRLIB_ObjectViewEnd();
		drawMesh(renderMesh, color);
	}

	void drawRenderMesh(const RenderMesh3D* renderMesh, uint32_t color = 0xffffffff) const {
		GRRLIB_ObjectViewBegin();
		GRRLIB_ObjectViewEnd();
		drawMesh(renderMesh, color);
	}

	void updateFrame() {
		GRRLIB_Render();
	}

	void drawText(uint32_t x, uint32_t y, const char* text, float size = 1, uint32_t color = 0x000000FF) {
		GRRLIB_2dMode();
		GRRLIB_SetTexture(font_texture, 0);
		GRRLIB_Printf(x, y, font_texture, color, size, text);
		//GRRLIB_PrintfTTF(x, y, font, text, size, color);
	}

private:
	GRRLIB_texImg* font_texture;

	Renderer() {
		GRRLIB_Settings.antialias = false;
		GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
		GRRLIB_SetLightAmbient(0xffffffff);
		GRRLIB_Camera3dSettings(0.0f, 0.0f, 5.0f, 0, 1, 0, 0, 0, 0);
		font_texture = GRRLIB_LoadTexture(font_png);
		GRRLIB_InitTileSet(font_texture, 16, 16, 32);
		//GRRLIB_2dMode();
		//GRRLIB_3dMode(0.1, 100, 60, true, false);
	}

	~Renderer() {
		GRRLIB_FreeTexture(font_texture);
	}

	void drawMesh(const RenderMesh3D* renderMesh, const uint32_t color) const {
		renderMesh->getTexture()->bind();
		Draw3D();
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

	void Draw3D() const {
		GRRLIB_3dMode(0.1, 100, 60, true, false);
		for (int i = 1; i <= 1; ++i) {
			GRRLIB_ObjectViewBegin();
			//GRRLIB_ObjectViewTrans(i, i, i);
			GRRLIB_ObjectViewEnd();
			GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 6);

			GX_Position3f32(0, 0, 0);
			GX_Color1u32(0xffffffff);
			GX_TexCoord2f32(0.0f, 0.0f);

			GX_Position3f32(1, 1, 0);
			GX_Color1u32(0xffffffff);
			GX_TexCoord2f32(1.0f, 1.0f);

			GX_Position3f32(0, 1, 0);
			GX_Color1u32(0xffffffff);
			GX_TexCoord2f32(0.0f, 1.0f);

			GX_Position3f32(0, 0, 0);
			GX_Color1u32(0xffffffff);
			GX_TexCoord2f32(0.0f, 0.0f);

			GX_Position3f32(1, 0, 0);
			GX_Color1u32(0xffffffff);
			GX_TexCoord2f32(1.0f, 0.0f);

			GX_Position3f32(1, 1, 0);
			GX_Color1u32(0xffffffff);
			GX_TexCoord2f32(1.0f, 1.0f);

			GX_End();
		}
	}

	Renderer(Renderer const&);
	void operator=(Renderer const&);
};
#endif // !RENDERER_H_
