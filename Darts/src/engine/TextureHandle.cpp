#include "Engine/TextureHandle.h"
#include "Engine/Engine.h"
#include "Engine/Renderer.h"
#include <ogc/cache.h>
#include <malloc.h>
#include <cstring>

TextureHandle::TextureHandle(void* pixels, uint32_t width, uint32_t height, bool repeat, bool antialias) {
	size_t size = width * height * 4;
	void* pixelData = memalign(32, size);
	GX_InitTexObjUserData(&m_textureHandle, pixelData);
	if (!pixelData) return;
	memcpy(pixelData, pixels, size);
	DCFlushRange(pixelData, size);
	if (repeat) {
		GX_InitTexObj(&m_textureHandle, pixelData, width, height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	}
	else
	{
		GX_InitTexObj(&m_textureHandle, pixelData, width, height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	}

	GXRModeObj& mode = Engine::GetRenderer().GetVideoMode();

	if (antialias) {
		GX_SetCopyFilter(mode.aa, mode.sample_pattern, GX_TRUE, mode.vfilter);
	}
	else
	{
		GX_InitTexObjLOD(&m_textureHandle, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
		GX_SetCopyFilter(GX_FALSE, mode.sample_pattern, GX_FALSE, mode.vfilter);
	}

	GX_InvalidateTexAll();
}

TextureHandle::~TextureHandle() {
	void* pixelData = GX_GetTexObjUserData(&m_textureHandle);
	if (pixelData) free(pixelData);
}

bool TextureHandle::Bind(uint8_t slot)
{
	void* pixelData = GX_GetTexObjUserData(&m_textureHandle);
	if (pixelData) {
		GX_LoadTexObj(&m_textureHandle, slot);
		return true;
	}
	else {
		return false;
	}
}
