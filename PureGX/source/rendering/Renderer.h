#ifndef RENDERER_H_
#define RENDERER_H_

#include <stdint.h>
#include <malloc.h>
#include <ogc/conf.h>
#include <ogc/machine/processor.h>
#include <gctypes.h>

#include "Mesh.h"
#include "Texture.h"
#include "../Vector3f.h"
#include "RenderMesh3D.h"
#include "font_png.h"
#include "RenderGlobal.h"

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
		drawMesh(renderMesh, color);
	}

	void drawRenderMesh(RenderMesh3D* renderMesh, uint32_t color = 0xffffffff) {
		drawMesh(renderMesh, color);
	}

	void swapFrameBuffer() {
		GX_DrawDone();
		_activeFramebuffer ^= 1;
		GX_CopyDisp(_frameBuffers[_activeFramebuffer], GX_TRUE);
		VIDEO_SetNextFramebuffer(_frameBuffers[_activeFramebuffer]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		if (_vmode->viTVMode & VI_NON_INTERLACE)
			VIDEO_WaitVSync();
	}

	void drawText(uint32_t x, uint32_t y, const char* text, float size = 1, uint32_t color = 0x000000FF) {

	}

	const GXRModeObj* getVideoMode() const {
		return _vmode;
	}

	void disableLights() {
		GX_SetNumTevStages(1);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetNumChans(1);
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		RenderGlobal::activeLights = 0;
	}

	void setAmbientLightColor(uint8_t r, uint8_t g, uint8_t b) {
		GX_SetChanAmbColor(GX_COLOR0A0, (GXColor) { r, g, b, 0xFF });
	}

	void setDiffuseLight(uint8_t num, Vector3f pos, float distance, float brightness, uint8_t r, uint8_t g, uint8_t b) {
		GXLightObj light;
		guVector lightPos = (guVector){ pos.x, pos.y, pos.z };
		guVecMultiply(_view, &lightPos, &lightPos);
		GX_InitLightPos(&light, lightPos.x, lightPos.y, lightPos.z);
		GX_InitLightColor(&light, (GXColor) { r, g, b, 0xFF });
		GX_InitLightSpot(&light, 0.0f, GX_SP_OFF);
		GX_InitLightDistAttn(&light, distance, brightness, GX_DA_MEDIUM);
		GX_LoadLightObj(&light, (1 << num));
		RenderGlobal::activeLights |= (1 << num);
		GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, RenderGlobal::activeLights, GX_DF_CLAMP, GX_AF_SPOT);
	}

private:
	GXRModeObj* _vmode;
	void* _frameBuffers[2];
	void* _fifoBuffer;
	Mtx _view;
	uint8_t _activeFramebuffer;

	Renderer() {
		// https://github.com/GRRLIB/GRRLIB/blob/master/GRRLIB/GRRLIB/GRRLIB_core.c
		// https://github.com/devkitPro/libogc/blob/master/libogc/video.c
		// https://devkitpro.org/wiki/libogc/GX

		VIDEO_Init();
		// disable display output
		VIDEO_SetBlack(true);

		// setup frame buffers
		_vmode = VIDEO_GetPreferredMode(NULL);

		// Video Mode Correction Taken from: https://github.com/GRRLIB/GRRLIB/blob/master/GRRLIB/GRRLIB/GRRLIB_core.c
		switch (_vmode->viTVMode) {
		case VI_DEBUG_PAL:  // PAL 50hz 576i
			_vmode = &TVPal528IntDf;
			break;
		default:
#ifdef HW_DOL
			if (VIDEO_HaveComponentCable())
				_vmode = &TVNtsc480Prog;
#endif
			break;
		}

#if defined(HW_RVL)
		if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
			_vmode->viWidth = 678;
		}
		else {    // 4:3
			_vmode->viWidth = 672;
		}
		// This probably needs to consider PAL
		_vmode->viXOrigin = (VI_MAX_WIDTH_NTSC - _vmode->viWidth) / 2;
#endif

#if defined(HW_RVL)
		// Patch widescreen on Wii U
		if (CONF_GetAspectRatio() == CONF_ASPECT_16_9 && (*(u32*)(0xCD8005A0) >> 16) == 0xCAFE)
		{
			write32(0xd8006a0, 0x30000004);
			mask32(0xd8006a8, 0, 2);
	}
#endif
		VIDEO_Configure(_vmode);
		RenderGlobal::mode = _vmode;
		_frameBuffers[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(_vmode));
		_frameBuffers[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(_vmode));
		_activeFramebuffer = 0;

		// Push Framebuffer to display
		VIDEO_SetNextFramebuffer(_frameBuffers[_activeFramebuffer]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		if (_vmode->viTVMode & VI_NON_INTERLACE)
			VIDEO_WaitVSync();

		// Setup & clear fifo buffer
		_fifoBuffer = memalign(32, DEFAULT_FIFO_SIZE);
		memset(_fifoBuffer, 0, DEFAULT_FIFO_SIZE);
		GX_Init(_fifoBuffer, DEFAULT_FIFO_SIZE);

		// Clear the background to black and clears the z-buffer
		GX_SetCopyClear((GXColor) { 0, 0, 0, 255 }, GX_MAX_Z24);

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

		GX_SetNumChans(1);    // colour is the same as vertex colour
		GX_SetNumTexGens(1);  // One texture exists
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		// setup render settings
		GX_SetViewport(0.0f, 0.0f, _vmode->fbWidth, _vmode->efbHeight, 0.0f, 1.0f);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetAlphaUpdate(GX_TRUE);
		GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetColorUpdate(GX_ENABLE);
		GX_SetCullMode(GX_CULL_NONE);
		GX_SetClipMode(GX_CLIP_ENABLE);
		GX_SetScissor(0, 0, _vmode->fbWidth, _vmode->efbHeight);
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetChanAmbColor(GX_COLOR0A0, (GXColor) { 0xff, 0xff, 0xff, 0xff });

		// setup vertex attribs
		GX_ClearVtxDesc();
		GX_InvVtxCache();
		GX_InvalidateTexAll();

		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		//GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

		// setup position/projection matrix
		guVector camPos = (guVector){ 0, 0, 5 };
		guVector upDir = (guVector){ 0, 1, 0 };
		guVector lookDir = (guVector){ 0, 0, 0 };

		Mtx44 m;
		guLookAt(_view, &camPos, &upDir, &lookDir);
		guPerspective(m, 60, (f32)_vmode->fbWidth / _vmode->efbHeight, 0.01, 100);
		GX_LoadProjectionMtx(m, GX_PERSPECTIVE);
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_LoadPosMtxImm(_view, GX_PNMTX0);

		// enable display output
		VIDEO_SetBlack(false);
}

	~Renderer() {
		GX_SetClipMode(GX_CLIP_DISABLE);
		GX_SetScissor(0, 0, _vmode->fbWidth, _vmode->efbHeight);
		GX_DrawDone();
		GX_AbortFrame();
		free(MEM_K1_TO_K0(_frameBuffers[0]));
		free(MEM_K1_TO_K0(_frameBuffers[1]));
		free(_fifoBuffer);
	}

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

	void DrawTestRect() const {
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

	Renderer(Renderer const&);
	void operator=(Renderer const&);
};
#endif // !RENDERER_H_
