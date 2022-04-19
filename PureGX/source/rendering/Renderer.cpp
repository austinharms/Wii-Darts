#include "Renderer.h"
#include "RenderGlobal.h"

#include "Texture.h"

Renderer::Renderer() {
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

Renderer::~Renderer() {
	GX_SetClipMode(GX_CLIP_DISABLE);
	GX_SetScissor(0, 0, _vmode->fbWidth, _vmode->efbHeight);
	GX_DrawDone();
	GX_AbortFrame();
	free(MEM_K1_TO_K0(_frameBuffers[0]));
	free(MEM_K1_TO_K0(_frameBuffers[1]));
	free(_fifoBuffer);
}

void Renderer::swapFrameBuffer() {
	GX_DrawDone();
	//GX_InvalidateTexAll();
	//_activeFramebuffer = !_activeFramebuffer;
	_activeFramebuffer ^= 1;

	//GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	//GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(_frameBuffers[_activeFramebuffer], GX_TRUE);
	VIDEO_SetNextFramebuffer(_frameBuffers[_activeFramebuffer]);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if (_vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
}

void Renderer::DrawTestRect() const {
	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 6);

	GX_Position3f32(0, 0, 0);
	GX_Color1u32(0xffffffff);
	//GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(1, 1, 0);
	GX_Color1u32(0xffffffff);
	//GX_TexCoord2f32(1.0f, 1.0f);

	GX_Position3f32(0, 1, 0);
	GX_Color1u32(0xffffffff);
	//GX_TexCoord2f32(0.0f, 1.0f);

	GX_Position3f32(0, 0, 0);
	GX_Color1u32(0xffffffff);
	//GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(1, 0, 0);
	GX_Color1u32(0xffffffff);
	//GX_TexCoord2f32(1.0f, 0.0f);

	GX_Position3f32(1, 1, 0);
	GX_Color1u32(0xffffffff);
	//GX_TexCoord2f32(1.0f, 1.0f);

	GX_End();
}