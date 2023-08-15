#include "engine/TextureHandle.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <ogc/cache.h>
#include <malloc.h>
#include <cstring>

TextureHandle::TextureHandle(void* pixels, uint16_t width, uint16_t height, bool repeat, bool antialias, WDPixelFormatEnum srcFormat) {
	size_t size = width * height * 4;
	void* pixelData = memalign(32, size);
	GX_InitTexObjUserData(&m_textureHandle, pixelData);
	if (!pixelData) return;

	if (srcFormat == WD_PIXEL_ARGB4X4) {
		memcpy(pixelData, pixels, size);
	}
	else {
		ConvertRGBA8PixelData((uint8_t*)pixelData, (uint32_t*)pixels, width, height);
	}

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

void TextureHandle::ConvertRGBA8PixelData(uint8_t* dst, uint32_t* src, uint16_t width, uint16_t height)
{
	uint32_t srcIndex = 0;
	for (uint16_t y = 0; y < height; ++y) {
		for (uint16_t x = 0; x < width; ++x) {
			uint32_t offset = (((y & (~3)) << 2) * width) + ((x & (~3)) << 4) + ((((y & 3) << 2) + (x & 3)) << 1);
			*((uint16_t*)(dst + offset)) = (uint16_t)(((src[srcIndex]) << 8) | ((src[srcIndex]) >> 24));
			*((uint16_t*)(dst + offset + 32)) = (uint16_t)((src[srcIndex]) >> 8);
			srcIndex += 1;
		}
	}
}
