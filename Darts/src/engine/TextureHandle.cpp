#include "engine/TextureHandle.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <ogc/cache.h>
#include <malloc.h>
#include <cstring>

TextureHandle::TextureHandle(const void* pixels, uint16_t width, uint16_t height, bool repeat, bool antialias, WDPixelFormatEnum srcFormat) {
	if (!pixels || width == 0 || height == 0) return;
	size_t size = (size_t)width * (size_t)height * 4;
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
	GX_InitTexObjUserData(&m_textureHandle, pixelData);
}

TextureHandle::~TextureHandle() {
	void* pixelData = GX_GetTexObjUserData(&m_textureHandle);
	if (pixelData) free(pixelData);
	GX_InitTexObjUserData(&m_textureHandle, nullptr);
}

bool TextureHandle::Bind(uint8_t slot)
{
	void* pixelData = GX_GetTexObjUserData(&m_textureHandle);
	if (pixelData) {
		GX_LoadTexObj(&m_textureHandle, GX_TEXMAP0 + slot);
		return true;
	}
	else {
		return false;
	}
}

void TextureHandle::ConvertRGBA8PixelData(uint8_t* dst, uint32_t* src, uint16_t width, uint16_t height)
{
	for (uint16_t y = 0; y < height; ++y) {
		for (uint16_t x = 0; x < width; ++x) {

			// Covert LE TO BE
			uint32_t srcPixel = src[x + (y * width)];
			//uint32_t srcPixel = src[x + ((height - 1 - y) * width)];
			uint8_t* srcPixelBuf = (uint8_t*)&srcPixel;
			uint8_t tmp = srcPixelBuf[0];
			srcPixelBuf[0] = srcPixelBuf[3];
			srcPixelBuf[3] = tmp;
			tmp = srcPixelBuf[1];
			srcPixelBuf[1] = srcPixelBuf[2];
			srcPixelBuf[2] = tmp;

			uint32_t offset = (((y & (~3)) << 2) * width) + ((x & (~3)) << 4) + ((((y & 3) << 2) + (x & 3)) << 1);
			*((uint16_t*)(dst + offset)) = (uint16_t)((srcPixel << 8) | (srcPixel >> 24));
			*((uint16_t*)(dst + offset + 32)) = (uint16_t)(srcPixel >> 8);
		}
	}

	//for (uint16_t x = 0; x < width; x += 4) {
	//	for (uint16_t y = 0; y < width; y += 4) {
	//		uint32_t square[16];
	//		uint32_t* squareItr = square;

	//		for (uint16_t subX = 0; subX < 4; subX++) {
	//			for (uint16_t subY = 0; subY < 4; subY++) {
	//				*squareItr = src[(x + (y * width)) + (subY + (subY * width))];
	//				squareItr += 1;
	//			}
	//		}

	//		for (uint16_t i = 0; i < 16; ++i) {
	//			*dst = (uint8_t)square[i];
	//			dst += 1;
	//			*dst = (uint8_t)(square[i] >> 24);
	//			dst += 1;
	//		}

	//		for (uint16_t i = 0; i < 16; ++i) {
	//			*dst = (uint8_t)(square[i] >> 16);
	//			dst += 1;
	//			*dst = (uint8_t)(square[i] >> 8);
	//			dst += 1;
	//		}
	//	}
	//}
}
