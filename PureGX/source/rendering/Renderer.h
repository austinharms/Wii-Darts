#ifndef RENDERER_H_
#define RENDERER_H_
//#include <grrlib.h>
#include <stdint.h>
#include <malloc.h>
#include <ogc/conf.h>
#include <ogc/machine/processor.h>

#include "Mesh.h"
#include "Texture.h"
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

	void drawRenderMesh(const RenderMesh3D* renderMesh, Vector3f position, uint32_t color = 0xffffffff) const {
		//GRRLIB_ObjectViewBegin();
		//GRRLIB_ObjectViewTrans(position.x, position.y, position.z);
		//GRRLIB_ObjectViewEnd();
		drawMesh(renderMesh, color);
	}

	void drawRenderMesh(const RenderMesh3D* renderMesh, uint32_t color = 0xffffffff) const {
		//GRRLIB_ObjectViewBegin();
		//GRRLIB_ObjectViewEnd();
		drawMesh(renderMesh, color);
	}

	void updateFrame() {
		//GRRLIB_Render();
	}

	void drawText(uint32_t x, uint32_t y, const char* text, float size = 1, uint32_t color = 0x000000FF) {
		//GRRLIB_2dMode();
		//GRRLIB_SetTexture(font_texture, 0);
		//GRRLIB_Printf(x, y, font_texture, color, size, text);
		//GRRLIB_PrintfTTF(x, y, font, text, size, color);
	}

private:
	GXRModeObj* _vmode;
	void* _frameBuffers[2];
	void* _fifoBuffer;
	Mtx _view;
	Mtx44 _projection;
	//GRRLIB_texImg* font_texture;

	Renderer() {
		// https://github.com/GRRLIB/GRRLIB/blob/master/GRRLIB/GRRLIB/GRRLIB_core.c
		// https://github.com/devkitPro/libogc/blob/master/libogc/video.c
		// https://devkitpro.org/wiki/libogc/GX

		VIDEO_Init();
		VIDEO_SetBlack(true);

		// setup frame buffers
		GXRModeObj* _vmode = VIDEO_GetPreferredMode(nullptr);
		VIDEO_Configure(_vmode);
		_frameBuffers[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(_vmode));
		_frameBuffers[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(_vmode));
		VIDEO_SetNextFramebuffer(_frameBuffers[0]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		if (_vmode->viTVMode & VI_NON_INTERLACE)
			VIDEO_WaitVSync();

		// setup & clear fifo buffer
		_fifoBuffer = memalign(32, DEFAULT_FIFO_SIZE);
		memset(_fifoBuffer, 0, DEFAULT_FIFO_SIZE);
		GX_Init(_fifoBuffer, DEFAULT_FIFO_SIZE);

		// Clear the background to opaque black and clears the z-buffer
		GX_SetCopyClear((GXColor) { 0, 0, 0, 0 }, GX_MAX_Z24);

		if (_vmode->aa) {
			GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);  // Set 16 bit RGB565
		}
		else {
			GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);  // Set 24 bit Z24
		}

		float fbWidth = GX_GetYScaleFactor(_vmode->efbHeight, _vmode->xfbHeight);
		float fbHeight = GX_SetDispCopyYScale(fbWidth);
		GX_SetDispCopySrc(0, 0, _vmode->fbWidth, _vmode->efbHeight);
		GX_SetDispCopyDst(_vmode->fbWidth, fbHeight);
		GX_SetCopyFilter(_vmode->aa, _vmode->sample_pattern, GX_TRUE, _vmode->vfilter);
		GX_SetFieldMode(_vmode->field_rendering, ((_vmode->viHeight == 2 * _vmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

		GX_SetDispCopyGamma(GX_GM_1_0);

		// setup vertex attribs
		GX_ClearVtxDesc();
		GX_InvVtxCache();
		GX_InvalidateTexAll();

		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_NRM, GX_NONE);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		//GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
		GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_TRUE);

		GX_SetNumChans(1);    // colour is the same as vertex colour
		GX_SetNumTexGens(1);  // One texture exists

		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		// setup view matrix
		guMtxIdentity(_view);
		guMtxTransApply(_view, _view, 0.0F, 0.0F, -100.0F);
		GX_LoadPosMtxImm(_view, GX_PNMTX0);

		// setup projection matrix
		guOrtho(_projection, 0.0f, _vmode->efbHeight, 0.0f, _vmode->fbWidth, 0.0f, 1000.0f);
		GX_LoadProjectionMtx(_projection, GX_ORTHOGRAPHIC);

		// setup render settings
		GX_SetViewport(0.0f, 0.0f, _vmode->fbWidth, _vmode->efbHeight, 0.0f, 1.0f);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetAlphaUpdate(GX_TRUE);
		GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetColorUpdate(GX_ENABLE);
		GX_SetCullMode(GX_CULL_NONE);


		VIDEO_SetBlack(false);

		// clear the screen
		GXColor background = { 0,0,0,0xff };
		GX_SetCopyClear(background, 0x00ffffff);


		//GRRLIB_Settings.antialias = false;
		//GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
		//GRRLIB_SetLightAmbient(0xffffffff);
		//GRRLIB_Camera3dSettings(0.0f, 0.0f, 5.0f, 0, 1, 0, 0, 0, 0);
		//font_texture = GRRLIB_LoadTexture(font_png);
		//GRRLIB_InitTileSet(font_texture, 16, 16, 32);
		//GRRLIB_2dMode();
		//GRRLIB_3dMode(0.1, 100, 60, true, false);
	}

	~Renderer() {
		//GRRLIB_FreeTexture(font_texture);
		GX_SetClipMode(GX_CLIP_DISABLE);
		GX_SetScissor(0, 0, _vmode->fbWidth, _vmode->efbHeight);
	}

	void drawMesh(const RenderMesh3D* renderMesh, const uint32_t color) const {
		renderMesh->getTexture()->bind();
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

	//void Draw3D() const {
	//	GRRLIB_3dMode(0.1, 100, 60, true, false);
	//	for (int i = 1; i <= 1; ++i) {
	//		GRRLIB_ObjectViewBegin();
	//		//GRRLIB_ObjectViewTrans(i, i, i);
	//		GRRLIB_ObjectViewEnd();
	//		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 6);

	//		GX_Position3f32(0, 0, 0);
	//		GX_Color1u32(0xffffffff);
	//		GX_TexCoord2f32(0.0f, 0.0f);

	//		GX_Position3f32(1, 1, 0);
	//		GX_Color1u32(0xffffffff);
	//		GX_TexCoord2f32(1.0f, 1.0f);

	//		GX_Position3f32(0, 1, 0);
	//		GX_Color1u32(0xffffffff);
	//		GX_TexCoord2f32(0.0f, 1.0f);

	//		GX_Position3f32(0, 0, 0);
	//		GX_Color1u32(0xffffffff);
	//		GX_TexCoord2f32(0.0f, 0.0f);

	//		GX_Position3f32(1, 0, 0);
	//		GX_Color1u32(0xffffffff);
	//		GX_TexCoord2f32(1.0f, 0.0f);

	//		GX_Position3f32(1, 1, 0);
	//		GX_Color1u32(0xffffffff);
	//		GX_TexCoord2f32(1.0f, 1.0f);

	//		GX_End();
	//	}
	//}

	Renderer(Renderer const&);
	void operator=(Renderer const&);
};
#endif // !RENDERER_H_
