#include "engine/Renderer.h"
#include "engine/RenderMeshHandle.h"
#include "engine/Engine.h"
#include "engine/LightEntity.h"
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

Renderer::Renderer() { 
	m_videoMode = nullptr;
	m_videoFIFO = nullptr;
	m_framebuffers[0] = nullptr;
	m_framebuffers[1] = nullptr;
	for (int i = 0; i < 8; ++i)
		m_lights[i] = nullptr;
	m_fov = 90;
	m_nearPlane = 0.01f;
	m_farPlane = 100;
	m_currentRenderMode = WD_RENDER_STACK;
	m_transformStackIndex = 0;
	m_activeFramebuffer = 0;
	m_activeDiffuseLights = 0;
	m_activeSpecularLights = 0;
	m_enabled = false;
	m_frameStarted = false;
	m_init = false;
}

Renderer::~Renderer() {
	if (m_init) {
		Disable();
		GX_SetClipMode(GX_CLIP_DISABLE);
		GX_SetScissor(0, 0, m_videoMode->fbWidth, m_videoMode->efbHeight);
		GX_DrawDone();
		GX_AbortFrame();
		free(MEM_K1_TO_K0(m_framebuffers[0]));
		free(MEM_K1_TO_K0(m_framebuffers[1]));
		free(m_videoFIFO);
		free(m_defaultTextureData);
	}
}

void Renderer::Init() {
	SetupVideo();
	SetupGX();
	SetupTEV();
	SetupVtxAttribs();
	SetupMatrices();
	m_defaultTextureData = memalign(32, 64);
	if (!m_defaultTextureData) {
		Engine::Log("Failed to allocate default texture");
		Engine::Quit();
		return;
	}

	memset(m_defaultTextureData, 0xff, 64);
	m_defaultTexture.Init(m_defaultTextureData, 4, 4, true, false);
	m_init = true;
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

const Transform& Renderer::GetCameraTransform()
{
	return m_viewTransform;
}

const Transform& Renderer::GetActiveCameraTransform()
{
	return m_transformStack[0];
}

const Transform& Renderer::GetActiveCameraNormalTransform()
{
	return m_viewNormalTransform;
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
	GX_SetCopyClear(((GXColor*)&color)[0], GX_MAX_Z24);
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
	Transform::Mul(m_transformStack[m_transformStackIndex], t, m_transformStack[++m_transformStackIndex]);
}

void Renderer::PushIdentityTransform()
{
	m_transformStack[++m_transformStackIndex] = m_transformStack[m_transformStackIndex - 1];
}

void Renderer::PopTransform()
{
	--m_transformStackIndex;
}

void Renderer::DrawRenderMesh(const RenderMeshHandle& mesh, TextureHandle* texture, uint32_t color, WdRenderModeEnum mode)
{
	if (!m_frameStarted || !mesh.GetValid()) return;
	if (!texture || !texture->Bind())
		m_defaultTexture.Bind();

	UpdateActiveMatrix(mode);
	SetMeshColor(color);
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
	Mtx tmp;
	guMtxCopy(m_viewTransform.GetMatrix(), m_viewNormalTransform.GetMatrix());
	guMtxInverse(m_viewNormalTransform.GetMatrix(), tmp);
	guMtxTranspose(tmp, m_viewNormalTransform.GetMatrix());
	GX_LoadNrmMtxImm(m_viewNormalTransform.GetMatrix(), WD_RENDER_VIEW);
	UpdateActiveMatrix(WD_RENDER_STACK);
	for (int i = 0; i < 8; ++i) {
		if (m_lights[i])
			m_lights[i]->UpdateLight();
	}

	m_frameStarted = true;
}

void Renderer::EndFrame()
{
	m_frameStarted = false;
	SwapBuffers();
}

void Renderer::SwapBuffers()
{
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(m_framebuffers[m_activeFramebuffer], GX_TRUE);
	GX_DrawDone();
	VIDEO_SetNextFramebuffer(m_framebuffers[m_activeFramebuffer]);
	VIDEO_Flush();
	WaitVSync();
	m_activeFramebuffer ^= 1;
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
	if (mode == WD_RENDER_STACK) {
		GX_LoadPosMtxImm(m_transformStack[m_transformStackIndex].GetMatrix(), WD_RENDER_STACK);
		Mtx nrm;
		Mtx tmp;
		guMtxCopy(m_transformStack[m_transformStackIndex].GetMatrix(), nrm);
		guMtxInverse(nrm, tmp);
		guMtxTranspose(tmp, nrm);
		GX_LoadNrmMtxImm(nrm, WD_RENDER_STACK);
	}

	if (mode != m_currentRenderMode) {
		if (mode == WD_RENDER_UI) {
			GX_LoadProjectionMtx(m_orthoProjection, GX_ORTHOGRAPHIC);
			GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_TRUE);
			DisableLights();
		}
		else if (m_currentRenderMode == WD_RENDER_UI) {
			GX_LoadProjectionMtx(m_perspecProjection, GX_PERSPECTIVE);
			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
			EnableLights();
		}

		m_currentRenderMode = mode;
		GX_SetCurrentMtx((uint32_t)mode);
	}
}

void Renderer::DrawIndexedMesh(const RenderMeshHandle& mesh)
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
			GX_Color1u32(0xFFFFFFFF);
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

void Renderer::DrawNonIndexedMesh(const RenderMeshHandle& mesh)
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
			GX_Color1u32(0xFFFFFFFF);
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
	GX_SetViewport(0.0f, m_videoMode->efbHeight, m_videoMode->fbWidth, -m_videoMode->efbHeight, 0.0f, 1.0f);
	//GX_SetCullMode(GX_CULL_FRONT);
	GX_SetCullMode(GX_CULL_BACK);
	//GX_SetCullMode(GX_CULL_NONE);
	GX_SetClipMode(GX_CLIP_ENABLE);
	//GX_SetClipMode(GX_CLIP_DISABLE);
	GX_SetChanAmbColor(GX_COLOR0A0, (GXColor) { 0x30, 0x30, 0x30, 0xff });
	GX_SetChanMatColor(GX_COLOR0A0, (GXColor) { 0xff, 0xff, 0xff, 0xff });
	GX_SetChanAmbColor(GX_COLOR1A1, (GXColor) { 0x00, 0x00, 0x00, 0x00 });
	GX_SetChanMatColor(GX_COLOR1A1, (GXColor) { 0xff, 0xff, 0xff, 0xff });
	SetClearColor(0x000000ff);
	SetMeshColor(0xffffffff);
}

void Renderer::SetupTEV()
{
	GX_SetNumChans(2);    // colour is the same as vertex colour
	GX_SetNumTexGens(1);  // One texture exists

	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_KONST, GX_CC_CPREV, GX_CC_ZERO);
	GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_KONST, GX_CA_APREV, GX_CA_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR1A1);
	GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_RASC, GX_CC_ONE, GX_CC_CPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_SetNumTevStages(3);
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
	Mtx nrm;
	Mtx tmp;
	guMtxCopy(t.GetMatrix(), nrm);
	guMtxInverse(nrm, tmp);
	guMtxTranspose(tmp, nrm);
	GX_LoadNrmMtxImm(nrm, WD_RENDER_IDENT);

	t.SetPosition((guVector) { 0, 0, -100 });
	GX_LoadPosMtxImm(t.GetMatrix(), WD_RENDER_UI);
	guMtxCopy(t.GetMatrix(), nrm);
	guMtxInverse(nrm, tmp);
	guMtxTranspose(tmp, nrm);
	GX_LoadNrmMtxImm(nrm, WD_RENDER_UI);

	guOrtho(m_orthoProjection, (float)m_videoMode->efbHeight, 0, 0, (float)m_videoMode->fbWidth, 0, 1000);

	guVector camPos = (guVector){ 0, 0, 1 };
	guVector lookPos = (guVector){ 0, 0, 0 };
	guVector up = (guVector){ 0, 1, 0 };
	m_viewTransform.Reset();
	m_viewTransform.SetPosition(camPos);
	m_viewTransform.LookAt(lookPos, up);
	UpdatePerspectiveProjectionMatrix();

	GX_SetCurrentMtx((uint32_t)WD_RENDER_STACK);
	UpdateActiveMatrix(WD_RENDER_STACK);
	EnableLights();
}

void Renderer::SetMeshColor(uint32_t color) {
	WD_STATIC_ASSERT(sizeof(color) == sizeof(GXColor), "GX Color and uint32_t size mismatch");
	GX_SetTevKColor(GX_KCOLOR0, ((GXColor*)&color)[0]);
}

void Renderer::DisableLights()
{
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
	GX_SetChanCtrl(GX_COLOR1A1, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
	GX_SetNumTevStages(2);
}

void Renderer::EnableLights()
{
	GX_SetChanCtrl(GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, m_activeDiffuseLights, GX_DF_CLAMP, GX_AF_SPOT);
	GX_SetChanCtrl(GX_ALPHA0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, m_activeDiffuseLights, GX_DF_NONE, GX_AF_NONE);
	GX_SetChanCtrl(GX_COLOR1, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, m_activeSpecularLights, GX_DF_CLAMP, GX_AF_SPEC);
	GX_SetChanCtrl(GX_ALPHA1, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, m_activeSpecularLights, GX_DF_NONE, GX_AF_NONE);
	GX_SetNumTevStages(3);
}

uint8_t Renderer::AcquireDiffuseLightIndex(LightEntity* entity) {
	for (int i = 0; i < 8; ++i) {
		uint8_t idx = 1 << i;
		if (!((m_activeDiffuseLights | m_activeSpecularLights) & idx)) {
			m_activeDiffuseLights |= idx;
			m_lights[i] = entity;
			return idx;
		}
	}

	return 0;
}

uint8_t Renderer::AcquireSpecularLightIndex(LightEntity* entity) {
	for (int i = 0; i < 8; ++i) {
		uint8_t idx = 1 << i;
		if (!((m_activeDiffuseLights | m_activeSpecularLights) & idx)) {
			m_activeSpecularLights |= idx;
			m_lights[i] = entity;
			return idx;
		}
	}

	return 0;
}

void Renderer::ReleaseDiffuseLightIndex(uint8_t index) {
	m_activeDiffuseLights &= ~index;
	m_lights[index] = nullptr;
}

void Renderer::ReleaseSpecularLightIndex(uint8_t index) {
	m_activeSpecularLights &= ~index;
	m_lights[index] = nullptr;
}

