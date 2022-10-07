#include "Renderer.h"

#include <new>
#include <stdint.h>
#include <malloc.h>
#include <ogc/conf.h>
#include <ogc/machine/processor.h>
#include <ogc/video.h>
#include <ogc/system.h>
#include <gctypes.h>
#include <cstring>

#include "Logger.h"

namespace wiidarts {
	Renderer::Renderer() {
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
		GX_SetCopyClear({ 0, 255, 0, 255 }, GX_MAX_Z24);

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
		GX_SetCullMode(GX_CULL_BACK);
		GX_SetClipMode(GX_CLIP_ENABLE);
		GX_SetScissor(0, 0, _vmode->fbWidth, _vmode->efbHeight);
		GX_SetChanAmbColor(GX_COLOR0A0, { 0xff, 0xff, 0xff, 0xff });

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

		// setup projection matrices
		guPerspective(_perspectiveMatrix, 90, (f32)_vmode->fbWidth / _vmode->efbHeight, 0.01, 100);
		guOrtho(_orthographicMatrix, (f32)_vmode->efbHeight, 0, 0, (f32)_vmode->fbWidth, 0, 100);
		GX_LoadProjectionMtx(_perspectiveMatrix, GX_PERSPECTIVE);
		_currentProjection = GX_PERSPECTIVE;
		// setup position matrices and transform stack
		Mtx identityMtx;
		guMtxIdentity(identityMtx);
		GX_LoadPosMtxImm(identityMtx, GX_PNMTX0);
		GX_LoadPosMtxImm(identityMtx, GX_PNMTX1);
		GX_LoadPosMtxImm(identityMtx, GX_PNMTX2);
		GX_SetCurrentMtx(GX_PNMTX0);
		_currentPositionMatrix = GX_PNMTX0;
		//_2DTransform.setPosition({ 0, 0, 0 });
		_2DTransform.lookAt({ 0, 0, 1 }, { 0, 1, 0 });
		//_cameraTransform.setPosition({ 0, 0, 0 });
		_cameraTransform.lookAt({ 0, 0, 1 }, { 0, 1, 0 });
		resetTransformStack();

		// enable display output
		VIDEO_SetBlack(false);
	}

	void Renderer::resetTransformStack()
	{
		while (!_transformStack.empty())
			_transformStack.pop();
		_transformStack.push(_cameraTransform);
		_transformStackChanged = true;
	}

	void Renderer::setPositionMatrix(uint32_t pnidx)
	{
		if (_currentPositionMatrix != pnidx) {
			_currentPositionMatrix = pnidx;
			GX_SetCurrentMtx(pnidx);
		}
	}

	void Renderer::setProjectionMatrix(uint8_t projection)
	{
		if (_currentProjection != projection) {
			if (projection == GX_PERSPECTIVE) {
				GX_LoadProjectionMtx(_perspectiveMatrix, GX_PERSPECTIVE);
			}
			else if (projection == GX_ORTHOGRAPHIC) {
				GX_LoadProjectionMtx(_orthographicMatrix, GX_ORTHOGRAPHIC);
			}
			else {
				Logger::error("Renderer::setProjectionMatrix invalid projection type expected GX_PERSPECTIVE or GX_ORTHOGRAPHIC");
			}

			_currentProjection = projection;
		}
	}

	void Renderer::setTransformStackPositionMatrix()
	{
		if (_transformStackChanged) {
			_transformStackChanged = false;
			GX_LoadPosMtxImm(_transformStack.top().getMatrix(), GX_PNMTX0);
		}

		setPositionMatrix(GX_PNMTX0);
	}

	void Renderer::setIdentityPositionMatrix()
	{
		setPositionMatrix(GX_PNMTX1);
	}

	void Renderer::setOtherPositionMatrix(const Transform& transform)
	{
		Transform t = _2DTransform * transform;
		GX_LoadPosMtxImm(t.getMatrix(), GX_PNMTX2);
		setPositionMatrix(GX_PNMTX2);
	}

	void Renderer::drawRawMesh3D(const Mesh& mesh, const uint32_t color)
	{
		setProjectionMatrix(GX_PERSPECTIVE);
		drawMesh(mesh, color);
	}

	void Renderer::drawRawMesh2D(const Mesh& mesh, const uint32_t color)
	{
		setProjectionMatrix(GX_ORTHOGRAPHIC);
		drawMesh(mesh, color);
	}

	Renderer::~Renderer() {
		GX_SetClipMode(GX_CLIP_DISABLE);
		GX_SetScissor(0, 0, _vmode->fbWidth, _vmode->efbHeight);
		VIDEO_SetBlack(true);
		GX_DrawDone();
		GX_AbortFrame();
		free(MEM_K1_TO_K0(_frameBuffers[0]));
		free(MEM_K1_TO_K0(_frameBuffers[1]));
		free(_fifoBuffer);
	}

	void Renderer::swapFrameBuffers()
	{
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(_frameBuffers[_activeFramebuffer], GX_TRUE);
		GX_DrawDone();

		VIDEO_SetNextFramebuffer(_frameBuffers[_activeFramebuffer]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		if (_vmode->viTVMode & VI_NON_INTERLACE)
			VIDEO_WaitVSync();
		_activeFramebuffer ^= 1;		// Flip framebuffer
		//GX_InvalidateTexAll(); // Fixes some texture garbles
		resetTransformStack();
	}

	void Renderer::pushTransform(const Transform& t)
	{
		_transformStack.push(t * _transformStack.top());
		_transformStackChanged = true;
	}

	void Renderer::popTransform()
	{
		_transformStack.pop();
		_transformStackChanged = true;
	}

	void Renderer::drawMesh3D(const Mesh& mesh, const uint32_t color)
	{
		setTransformStackPositionMatrix();
		drawRawMesh3D(mesh, color);
	}

	void Renderer::drawMesh2D(const Mesh& mesh, const uint32_t color)
	{
		setTransformStackPositionMatrix();
		drawRawMesh2D(mesh, color);
	}

	void Renderer::drawMesh(const Mesh& mesh, const uint32_t color)
	{
		if (!mesh.getValid()) return;
		uint16_t indexCount = mesh.getIndexCount();
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, indexCount);
		constexpr uint32_t vertexStride = Mesh::FLOATS_PER_VERTEX;
		const float* verticies = mesh.getVertexBufferPtr();
		const uint16_t* indicies = mesh.getIndexBufferPtr();
		for (uint16_t i = 0; i < indexCount; ++i) {
			const float* vertexPtr = verticies + (vertexStride * indicies[i]);
			GX_Position3f32(vertexPtr[0], vertexPtr[1], vertexPtr[2]);
			GX_Color1u32(color);
			GX_TexCoord2f32(vertexPtr[3], vertexPtr[4]);
		}

		GX_End();
	}

	void Renderer::drawMesh3D(const Mesh& mesh, const Transform& transform, const uint32_t color)
	{
		setOtherPositionMatrix(transform);
		drawRawMesh3D(mesh, color);
	}

	void Renderer::drawMesh2D(const Mesh& mesh, const Transform& transform, const uint32_t color)
	{
		setOtherPositionMatrix(transform);
		drawRawMesh2D(mesh, color);
	}

	void Renderer::drawMeshIdentity3D(const Mesh& mesh, const uint32_t color)
	{
		setIdentityPositionMatrix();
		drawRawMesh3D(mesh, color);
	}

	void Renderer::drawMeshIdentity2D(const Mesh& mesh, const uint32_t color)
	{
		setIdentityPositionMatrix();
		drawRawMesh2D(mesh, color);
	}

	uint16_t Renderer::getScreenWidth() const
	{
		return  _vmode->fbWidth;
	}

	uint16_t Renderer::getScreenHeight() const
	{
		return _vmode->efbHeight;
	}

	GXRModeObj* Renderer::getMode() const
	{
		return _vmode;
	}
}
