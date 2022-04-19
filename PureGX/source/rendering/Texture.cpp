#include "Texture.h"

#include <malloc.h>

#include "RenderGlobal.h"

Texture::Texture(const void* data, bool repeat, bool antialias) {
	_pixels = (uint8_t*)memalign(32, 4);
	_width = 1;
	_height = 1;
	_pixels[0] = 0xff;
	_pixels[1] = 0x00;
	_pixels[2] = 0x00;
	_pixels[3] = 0xff;
	DCFlushRange(_pixels, ((uint32_t)_width * (uint32_t)_height) << 2);
	if (repeat) {
		GX_InitTexObj(&_texture, _pixels, _width, _height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	}
	else
	{
		GX_InitTexObj(&_texture, _pixels, _width, _height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	}

	if (antialias) {
		GX_SetCopyFilter(RenderGlobal::mode->aa, RenderGlobal::mode->sample_pattern, GX_TRUE, RenderGlobal::mode->vfilter);
	}
	else
	{
		GX_InitTexObjLOD(&_texture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
		GX_SetCopyFilter(GX_FALSE, RenderGlobal::mode->sample_pattern, GX_FALSE, RenderGlobal::mode->vfilter);
	}
}

Texture::~Texture() {
	free(_pixels);
}

uint16_t Texture::getHeight() const {
	return _height;
}

uint16_t Texture::getWidth() const {
	return _width;
}

void Texture::bind() {
	GX_LoadTexObj(&_texture, GX_TEXMAP0);
	//GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	//GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}