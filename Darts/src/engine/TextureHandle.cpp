#include "engine/TextureHandle.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <ogc/cache.h>
#include <cstring>

void TextureHandle::ConvertRGBA8PixelData(uint8_t* dst, uint32_t* src, uint16_t width, uint16_t height)
{
	uint32_t srcItr = 0;
	for (uint16_t y = 0; y < height; ++y) {
		for (uint16_t x = 0; x < width; ++x) {
			uint32_t srcPixel = src[srcItr++];
			uint32_t offset = (((y & (~3)) << 2) * width) + ((x & (~3)) << 4) + ((((y & 3) << 2) + (x & 3)) << 1);
			*((uint16_t*)(dst + offset)) = (uint16_t)((srcPixel << 8) | (srcPixel >> 24));
			*((uint16_t*)(dst + offset + 32)) = (uint16_t)(srcPixel >> 8);
		}
	}
}

TextureHandle::TextureHandle() {
	memset(&m_textureHandle, 0, sizeof(GXTexObj));
}

TextureHandle::~TextureHandle() {
	memset(&m_textureHandle, 0, sizeof(GXTexObj));
}

bool TextureHandle::Init(void* pixels, uint16_t width, uint16_t height, bool repeat, bool antialias) {
	memset(&m_textureHandle, 0, sizeof(GXTexObj));
	if (!pixels || width == 0 || height == 0) return false;
	size_t size = (size_t)width * (size_t)height * 4;
	DCFlushRange(pixels, size);
	if (repeat) {
		GX_InitTexObj(&m_textureHandle, pixels, width, height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	}
	else
	{
		GX_InitTexObj(&m_textureHandle, pixels, width, height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
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

	GX_InitTexObjUserData(&m_textureHandle, pixels);
	GX_InvalidateTexAll();
	return true;
}

WD_NODISCARD bool TextureHandle::GetValid()
{
	return GX_GetTexObjUserData(&m_textureHandle) != nullptr;
}

bool TextureHandle::Bind(uint8_t slot)
{
	bool valid = GetValid();
	if (valid) GX_LoadTexObj(&m_textureHandle, slot);
	return valid;
}
