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

uint32_t defaultTextureColor = 0xffffffff;

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
	m_transformStackIndex = 0;
	m_frameStarted = false;
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
	SetupTEV();
	SetupVtxAttribs();
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

void Renderer::SetCameraTransform(const Transform& t)
{
	m_viewTransform = t;
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

void Renderer::DrawRenderMesh(const RenderMesh& mesh, TextureHandle* texture)
{
	if (!m_frameStarted || !mesh.GetValid()) return;
	if (!texture) {
		m_defaultTexture.Bind();
	}
	else {
		texture->Bind();
	}

	PushActiveTransform();
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
	guVector camPos = (guVector){ 0, 0, 1 };
	guVector lookPos = (guVector){ 0, 0, 0 };
	guVector up = (guVector){ 0, 1, 0 };
	Transform t;
	t.SetPosition(camPos);
	t.LookAt(lookPos, up);
	SetCameraTransform(t);
}

void Renderer::PushActiveTransform()
{
	GX_LoadPosMtxImm(m_transformStack[m_transformStackIndex].GetMatrix(), GX_PNMTX0);
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

		if (format & RMF_HAS_VERTEX_UVS) {
			GX_TexCoord2f32(vertexItr[0], vertexItr[1]);
			vertexItr += 2;
		}
		else {
			GX_TexCoord2f32(0.0f, 0.0f);
		}

		if (format & RMF_HAS_VERTEX_COLOR) {
			GX_Color1u32(((uint32_t*)vertexItr)[0]);
			vertexItr += 1;
		}
		else {
			GX_Color1u32(0xffffffff);
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
			GX_Color1u32(0xff0000ff);
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

void Renderer::StartFrame()
{
	m_transformStackIndex = 0;
	m_transformStack[0] = m_viewTransform;
	PushActiveTransform();
	m_frameStarted = true;
}

void Renderer::EndFrame()
{
	m_frameStarted = false;
	SwapBuffers();
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
	GX_SetViewport(0.0f, 0.0f, m_videoMode->fbWidth, m_videoMode->efbHeight, 0.0f, 1.0f);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetColorUpdate(GX_ENABLE);
	//GX_SetCullMode(GX_CULL_NONE);
	GX_SetCullMode(GX_CULL_FRONT);
	GX_SetClipMode(GX_CLIP_ENABLE);
	GX_SetScissor(0, 0, m_videoMode->fbWidth, m_videoMode->efbHeight);
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetChanAmbColor(GX_COLOR0A0, (GXColor) { 0xff, 0xff, 0xff, 0xff });
}

void Renderer::SetupTEV()
{
	GX_SetNumChans(1);    // colour is the same as vertex colour
	GX_SetNumTexGens(1);  // One texture exists
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	//GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	//Mtx mv;
	//Mtx mr;
	//guLightPerspective(mv, 45, (f32)m_videoMode->fbWidth / (f32)m_videoMode->efbHeight, 1.05F, 1.0F, 0.0F, 0.0F);
	//guMtxTrans(mr, 0.0F, 0.0F, -1.0F);
	//guMtxConcat(mv, mr, mv);
	//GX_LoadTexMtxImm(mv, GX_TEXMTX0, GX_MTX3x4);

	//GX_SetTevColor(GX_CC_C2, (GXColor) { 0xff, 0xff, 0xff, 0xff });

	//GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C2, GX_CC_RASC, GX_CC_ZERO);
	//GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_RASC, GX_CC_ZERO);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	//GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	//GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
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
