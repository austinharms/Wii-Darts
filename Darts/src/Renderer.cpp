#include "renderer.h"
#include <ogc/video.h>
#include <ogc/system.h>
#include <gctypes.h>
#include <ogc/gx.h>
#include <ogc/conf.h>
#include <ogc/machine/processor.h>
#include <gctypes.h>
#include <malloc.h>
#include <cstring>

using namespace darts;

#define DEFAULT_FIFO_SIZE (256 * 1024)

uint32_t defaultTextureColor = 0xFFFFFFFF;

Renderer::Renderer() : m_defaultTexture(&defaultTextureColor, 1, 1, true, false) {
	m_videoMode = nullptr;
	m_videoFIFO = nullptr;
	m_framebuffers[0] = nullptr;
	m_framebuffers[1] = nullptr;
	m_activeFramebuffer = 0;
	m_enabled = false;
	m_nearPlane = 0.01f;
	m_farPlane = 100;
	m_fov = 90;
	VIDEO_Init();
	Disable();
	m_videoMode = VIDEO_GetPreferredMode(NULL);
	CorrectVideoMode();
	VIDEO_Configure(m_videoMode);
	m_framebuffers[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(m_videoMode));
	m_framebuffers[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(m_videoMode));
	m_activeFramebuffer = 0;
	VIDEO_SetNextFramebuffer(m_framebuffers[m_activeFramebuffer]);
	VIDEO_Flush();
	WaitVSync();
	SetupGX();
	UpdateProjectionMatrix();
	ResetViewMatrix();
}

Renderer::~Renderer() {
	Disable();
	GX_SetClipMode(GX_CLIP_DISABLE);
	GX_SetScissor(0, 0, m_videoMode->fbWidth, m_videoMode->efbHeight);
	GX_DrawDone();
	GX_AbortFrame();
	free(MEM_K1_TO_K0(m_framebuffers[0]));
	free(MEM_K1_TO_K0(m_framebuffers[1]));
	free(m_videoFIFO);
}

void Renderer::GetFramebufferSize(uint16_t* width, uint16_t* height) const
{
	if (width) *width = m_videoMode->fbWidth;
	if (height) *height = m_videoMode->efbHeight;
}

void Renderer::SetCameraTransform(Mtx transform)
{
	GX_LoadPosMtxImm(transform, GX_PNMTX0);
}

void Renderer::SetFOV(float fov)
{
	m_fov = fov;
	UpdateProjectionMatrix();
}

void Renderer::SetClippingPlanes(float near, float far)
{
	m_nearPlane = near;
	m_farPlane = far;
	UpdateProjectionMatrix();
}

void Renderer::SetClearColor(uint32_t color)
{
	WD_STATIC_ASSERT(sizeof(color) == sizeof(GXColor), "GX Color and uint32_t size mismatch");
	GXColor gxColor;
	memcpy(&gxColor, &color, sizeof(GXColor));
	GX_SetCopyClear(gxColor, GX_MAX_Z24);
}

GXRModeObj& Renderer::GetVideoMode()
{
	return *m_videoMode;
}

void Renderer::Enabled()
{
	VIDEO_SetBlack(false);
	m_enabled = true;
}

void Renderer::Disable()
{
	VIDEO_SetBlack(true);
	m_enabled = false;
}

void Renderer::SwapBuffers()
{
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(m_framebuffers[m_activeFramebuffer], GX_TRUE);
	GX_DrawDone();
	VIDEO_SetNextFramebuffer(m_framebuffers[m_activeFramebuffer]);
	VIDEO_Flush();
	WaitVSync();
	m_activeFramebuffer ^= 1;
	//GX_InvalidateTexAll(); // Fixes some texture garbles
}

void Renderer::UpdateProjectionMatrix()
{
	Mtx44 m;
	uint16_t w, h;
	GetFramebufferSize(&w, &h);
	guPerspective(m, m_fov, (f32)w / (f32)h, m_nearPlane, m_farPlane);
	GX_LoadProjectionMtx(m, GX_PERSPECTIVE);
}

void Renderer::ResetViewMatrix()
{
	Mtx44 m;
	guMtx44Identity(m);
	guVector camPos = (guVector){ 0, 0, 1 };
	guVector lookDir = (guVector){ 0, 0, 0 };
	guVector up = (guVector){ 0, 1, 0 };
	guLookAt(m, &camPos, &up, &lookDir);
	SetCameraTransform(m);
}

void Renderer::DrawTestRect()
{
	float x = 0;
	float y = 0;
	float z = 0;
	uint32_t color = 0x0000ffff;

	m_defaultTexture.Bind();

	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 6);

	GX_Position3f32(0 + x, 0 + y, 0 + z);
	GX_Color1u32(color);
	GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(0.05f + x, 0.05f + y, 0 + z);
	GX_Color1u32(color);
	GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(0 + x, 0.05f + y, 0 + z);
	GX_Color1u32(color);
	GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(0 + x, 0 + y, 0 + z);
	GX_Color1u32(color);
	GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(0.05f + x, 0 + y, 0 + z);
	GX_Color1u32(color);
	GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(0.05f + x, 0.05f + y, 0 + z);
	GX_Color1u32(color);
	GX_TexCoord2f32(0.0f, 0.0f);

	GX_End();
}

void Renderer::CorrectVideoMode()
{
	switch (m_videoMode->viTVMode) {
	case VI_DEBUG_PAL:  // PAL 50hz 576i
		m_videoMode = &TVPal528IntDf;
		break;
	default:
#ifdef HW_DOL
		if (VIDEO_HaveComponentCable())
			m_videoMode = &TVNtsc480Prog;
#endif
		break;
	}

#if defined(HW_RVL)
	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
		m_videoMode->viWidth = 678;
	}
	else {    // 4:3
		m_videoMode->viWidth = 672;
	}
	// This probably needs to consider PAL
	m_videoMode->viXOrigin = (VI_MAX_WIDTH_NTSC - m_videoMode->viWidth) / 2;
#endif

#if defined(HW_RVL)
	// Patch widescreen on Wii U
	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9 && (*(u32*)(0xCD8005A0) >> 16) == 0xCAFE)
	{
		write32(0xd8006a0, 0x30000004);
		mask32(0xd8006a8, 0, 2);
	}
#endif
}

void Renderer::WaitVSync() const
{
	VIDEO_WaitVSync();
	if (m_videoMode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
}

void Renderer::SetupGX()
{
	m_videoFIFO = memalign(32, DEFAULT_FIFO_SIZE);
	memset(m_videoFIFO, 0, DEFAULT_FIFO_SIZE);
	GX_Init(m_videoFIFO, DEFAULT_FIFO_SIZE);
	GX_SetCopyClear((GXColor) { 0, 0, 0, 255 }, GX_MAX_Z24);
	if (m_videoMode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);  // Set 16 bit RGB565
	}
	else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);  // Set 24 bit Z24
	}

	float fbWidth = GX_GetYScaleFactor(m_videoMode->efbHeight, m_videoMode->xfbHeight);
	float fbHeight = GX_SetDispCopyYScale(fbWidth);
	GX_SetDispCopySrc(0, 0, m_videoMode->fbWidth, m_videoMode->efbHeight);
	GX_SetDispCopyDst(m_videoMode->fbWidth, fbHeight);
	GX_SetCopyFilter(m_videoMode->aa, m_videoMode->sample_pattern, GX_TRUE, m_videoMode->vfilter);
	GX_SetFieldMode(m_videoMode->field_rendering, ((m_videoMode->viHeight == 2 * m_videoMode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_SetNumChans(1);    // colour is the same as vertex colour
	GX_SetNumTexGens(1);  // One texture exists
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	// setup render settings
	GX_SetViewport(0.0f, 0.0f, m_videoMode->fbWidth, m_videoMode->efbHeight, 0.0f, 1.0f);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetColorUpdate(GX_ENABLE);
	GX_SetCullMode(GX_CULL_FRONT);
	GX_SetClipMode(GX_CLIP_ENABLE);
	GX_SetScissor(0, 0, m_videoMode->fbWidth, m_videoMode->efbHeight);
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
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
}
