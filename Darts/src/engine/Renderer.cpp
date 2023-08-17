#include "engine/Renderer.h"
#include "engine/RenderMesh.h"
#include <ogc/video.h>
#include <ogc/system.h>
#include <gctypes.h>
#include <ogc/gx.h>
#include <ogc/conf.h>
#include <ogc/machine/processor.h>
#include <gctypes.h>
#include <malloc.h>
#include <cstring>

#define DEFAULT_FIFO_SIZE (256 * 1024)

uint32_t defaultTexture[16] = {
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff
};

Renderer::Renderer() : m_defaultTexture(&defaultTexture, 4, 4, false, false, WD_PIXEL_RGBA8) {
	m_videoMode = nullptr;
	m_videoFIFO = nullptr;
	m_framebuffers[0] = nullptr;
	m_framebuffers[1] = nullptr;
	m_activeFramebuffer = 0;
	m_enabled = false;
	m_nearPlane = 0.01f;
	m_farPlane = 100;
	m_fov = 90;
	m_transformStackIndex = 0;
	m_frameStarted = false;
	m_currentRenderMode = WD_RENDER_STACK;
	SetupVideo();
	SetupGX();
	SetupTEV();
	SetupVtxAttribs();
	SetupMatrices();
	m_gui.Init();
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

GXRModeObj& Renderer::GetVideoMode()
{
	return *m_videoMode;
}

void Renderer::GetFramebufferSize(uint16_t* width, uint16_t* height) const
{
	if (width) *width = m_videoMode->fbWidth;
	if (height) *height = m_videoMode->efbHeight;
}

void Renderer::SetCameraTransform(const Transform& t)
{
	m_viewTransform = t;
}

void Renderer::SetFOV(float fov)
{
	m_fov = fov;
	UpdatePerspectiveProjectionMatrix();
}

void Renderer::SetClippingPlanes(float near, float far)
{
	m_nearPlane = near;
	m_farPlane = far;
	UpdatePerspectiveProjectionMatrix();
}

void Renderer::SetClearColor(uint32_t color)
{
	WD_STATIC_ASSERT(sizeof(color) == sizeof(GXColor), "GX Color and uint32_t size mismatch");
	GXColor gxColor;
	memcpy(&gxColor, &color, sizeof(GXColor));
	GX_SetCopyClear(gxColor, GX_MAX_Z24);
}

void Renderer::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	GX_SetScissor(x, y, width, height);
}

void Renderer::ResetScissor()
{
	uint16_t w, h;
	GetFramebufferSize(&w, &h);
	SetScissor(0, 0, w, h);
}

void Renderer::PushTransform(const Transform& t)
{
	Transform::Mul(t, m_transformStack[m_transformStackIndex], m_transformStack[++m_transformStackIndex]);
}

void Renderer::PushIdentityTransform()
{
	m_transformStack[++m_transformStackIndex] = m_transformStack[m_transformStackIndex - 1];
}

void Renderer::PopTransform()
{
	--m_transformStackIndex;
}

void Renderer::DrawRenderMesh(const RenderMesh& mesh, TextureHandle* texture, WdRenderModeEnum mode)
{
	if (!m_frameStarted || !mesh.GetValid()) return;
	if (!texture) {
		m_defaultTexture.Bind();
	}
	else {
		texture->Bind();
	}

	//m_gui.GetFontAtlasTexture()->Bind();
	UpdateActiveMatrix(mode);
	if (mesh.GetFormat() & RMF_HAS_INDICES) {
		DrawIndexedMesh(mesh);
	}
	else {
		DrawNonIndexedMesh(mesh);
	}
}

void Renderer::Enable()
{
	VIDEO_SetBlack(false);
	m_enabled = true;
}

void Renderer::Disable()
{
	VIDEO_SetBlack(true);
	m_enabled = false;
}

void Renderer::StartFrame()
{
	m_transformStackIndex = 0;
	m_transformStack[0] = m_viewTransform;
	GX_LoadPosMtxImm(m_viewTransform.GetMatrix(), WD_RENDER_VIEW);
	GX_LoadPosMtxImm(m_transformStack[0].GetMatrix(), WD_RENDER_STACK);
	ResetScissor();
	SetupTEV();
	m_gui.StartFrame();
	m_frameStarted = true;
}

void Renderer::EndFrame()
{
	m_gui.RenderUI();
	m_frameStarted = false;
	SwapBuffers();
}

void Renderer::SwapBuffers()
{
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_SET);
	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(m_framebuffers[m_activeFramebuffer], GX_TRUE);
	GX_DrawDone();
	VIDEO_SetNextFramebuffer(m_framebuffers[m_activeFramebuffer]);
	VIDEO_Flush();
	WaitVSync();
	m_activeFramebuffer ^= 1;
	//GX_InvalidateTexAll();
}

void Renderer::WaitVSync() const
{
	VIDEO_WaitVSync();
	if (m_videoMode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
}

void Renderer::UpdatePerspectiveProjectionMatrix()
{
	uint16_t w, h;
	GetFramebufferSize(&w, &h);
	guPerspective(m_perspecProjection, m_fov, (f32)w / (f32)h, m_nearPlane, m_farPlane);
	if (m_currentRenderMode != WD_RENDER_UI) GX_LoadProjectionMtx(m_perspecProjection, GX_PERSPECTIVE);
}

void Renderer::UpdateActiveMatrix(WdRenderModeEnum mode)
{
	if (mode == WD_RENDER_STACK) GX_LoadPosMtxImm(m_transformStack[m_transformStackIndex].GetMatrix(), WD_RENDER_STACK);
	if (mode != m_currentRenderMode) {
		if (mode == WD_RENDER_UI) {
			GX_LoadProjectionMtx(m_orthoProjection, GX_ORTHOGRAPHIC);
		}
		else if (m_currentRenderMode == WD_RENDER_UI) {
			GX_LoadProjectionMtx(m_perspecProjection, GX_PERSPECTIVE);
		}

		m_currentRenderMode = mode;
		GX_SetCurrentMtx((uint32_t)mode);
	}
}

void Renderer::DrawIndexedMesh(const RenderMesh& mesh)
{
	const float* vertexData = (float*)mesh.GetVertexBuffer();
	const uint16_t* indexData = mesh.GetIndexBuffer();
	uint16_t primCount = mesh.GetIndexCount();
	RenderMeshFormat format = mesh.GetFormat();

	uint8_t vertexStride = sizeof(float) * 3;
	if (format & RMF_HAS_VERTEX_NORMAL)
		vertexStride += sizeof(float) * 3;
	if (format & RMF_HAS_VERTEX_UVS)
		vertexStride += sizeof(float) * 2;
	if (format & RMF_HAS_VERTEX_COLOR)
		vertexStride += sizeof(uint32_t);

	vertexStride /= sizeof(float);

	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, primCount);
	for (uint16_t i = 0; i < primCount; ++i) {
		const float* vertexItr = vertexData + (vertexStride * indexData[i]);
		GX_Position3f32(vertexItr[0], vertexItr[1], vertexItr[2]);
		vertexItr += 3;

		if (format & RMF_HAS_VERTEX_NORMAL) {
			GX_Normal3f32(vertexItr[0], vertexItr[1], vertexItr[2]);
			vertexItr += 3;
		}
		else {
			GX_Normal3f32(0, 0, 1);
		}

		if (format & RMF_HAS_VERTEX_COLOR) {
			GX_Color1u32(((uint32_t*)vertexItr)[0]);
			vertexItr += 1;
		}
		else {
			GX_Color1u32(0xffffffff);
		}

		if (format & RMF_HAS_VERTEX_UVS) {
			GX_TexCoord2f32(vertexItr[0], vertexItr[1]);
			vertexItr += 2;
		}
		else {
			GX_TexCoord2f32(0.0f, 0.0f);
		}
	}

	GX_End();
}

void Renderer::DrawNonIndexedMesh(const RenderMesh& mesh)
{
	const float* vertexItr = (float*)mesh.GetVertexBuffer();
	uint16_t primCount = mesh.GetVertexCount();
	RenderMeshFormat format = mesh.GetFormat();
	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, primCount);
	for (uint16_t i = 0; i < primCount; ++i) {
		GX_Position3f32(vertexItr[0], vertexItr[1], vertexItr[2]);
		vertexItr += 3;

		if (format & RMF_HAS_VERTEX_NORMAL) {
			GX_Normal3f32(vertexItr[0], vertexItr[1], vertexItr[2]);
			vertexItr += 3;
		}
		else {
			GX_Normal3f32(0, 0, 1);
		}

		if (format & RMF_HAS_VERTEX_COLOR) {
			GX_Color1u32(((uint32_t*)vertexItr)[0]);
			vertexItr += 1;
		}
		else {
			GX_Color1u32(0xffffffff);
		}

		if (format & RMF_HAS_VERTEX_UVS) {
			GX_TexCoord2f32(vertexItr[0], vertexItr[1]);
			vertexItr += 2;
		}
		else {
			GX_TexCoord2f32(0.5f, 0.5f);
		}
	}

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

void Renderer::SetupVideo()
{
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
}

void Renderer::SetupGX()
{
	m_videoFIFO = memalign(32, DEFAULT_FIFO_SIZE);
	memset(m_videoFIFO, 0, DEFAULT_FIFO_SIZE);
	GX_Init(m_videoFIFO, DEFAULT_FIFO_SIZE);
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
	//GX_SetViewport(0.0f, 0, m_videoMode->fbWidth, m_videoMode->efbHeight, 0.0f, 1.0f);
	GX_SetViewport(0.0f, m_videoMode->efbHeight, m_videoMode->fbWidth, -m_videoMode->efbHeight, 0.0f, 1.0f);	
	GX_SetCullMode(GX_CULL_NONE);
	//GX_SetCullMode(GX_CULL_BACK);
	//GX_SetClipMode(GX_CLIP_DISABLE);
	GX_SetClipMode(GX_CLIP_ENABLE);
	GX_SetChanAmbColor(GX_COLOR0A0, (GXColor) { 0xff, 0xff, 0xff, 0xff });
	SetClearColor(0x000000ff);
}

void Renderer::SetupTEV()
{
	GX_SetNumChans(1);    // colour is the same as vertex colour
	GX_SetNumTexGens(1);  // One texture exists
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	//GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_TEXMTX0);
	Mtx mv;
	Mtx mr;
	guLightPerspective(mv, 45, (f32)m_videoMode->fbWidth / (f32)m_videoMode->efbHeight, 1.05F, 1.0F, 0.0F, 0.0F);
	guMtxTrans(mr, 0.0F, 0.0F, -1.0F);
	guMtxConcat(mv, mr, mv);
	GX_LoadTexMtxImm(mv, GX_TEXMTX0, GX_MTX3x4);

	//GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
	//GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	//GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	//GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	//GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	//GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	//GX_SetTevOp(GX_TEVSTAGE1, GX_BLEND);
	//GX_SetNumTevStages(2);
}

void Renderer::SetupVtxAttribs()
{
	GX_ClearVtxDesc();
	GX_InvVtxCache();
	GX_InvalidateTexAll();

	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void Renderer::SetupMatrices()
{
	Transform t;
	GX_LoadPosMtxImm(t.GetMatrix(), WD_RENDER_IDENT);
	GX_LoadPosMtxImm(t.GetMatrix(), WD_RENDER_UI);
	guOrtho(m_orthoProjection, (float)m_videoMode->efbHeight, 0, 0, (float)m_videoMode->fbWidth, 0, 100);
	
	guVector camPos = (guVector){ 0, 0, 1 };
	guVector lookPos = (guVector){ 0, 0, 0 };
	guVector up = (guVector){ 0, 1, 0 };
	m_viewTransform.Reset();
	m_viewTransform.SetPosition(camPos);
	m_viewTransform.LookAt(lookPos, up);
	UpdatePerspectiveProjectionMatrix();

	GX_SetCurrentMtx((uint32_t)WD_RENDER_STACK);
	UpdateActiveMatrix(WD_RENDER_STACK);
}
