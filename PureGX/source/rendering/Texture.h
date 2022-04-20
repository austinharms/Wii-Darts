#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <stdint.h>
#include <malloc.h>

#include "RenderGlobal.h"

class Texture
{
public:
	Texture(const void* data, bool repeat = false, bool antialias = false) {
		_pixels = nullptr;
		loadTexture((const uint8_t)data);
		updateTexture(repeat, antialias);
	}

	~Texture() {
		free(_pixels);
	}

	uint16_t getHeight() const {
		return _height;
	}

	uint16_t getWidth() const {
		return _width;
	}

	void bind() {
		GX_LoadTexObj(&_texture, GX_TEXMAP0);
	}

	void loadTexture(const uint8_t* data) {
		free(_pixels);
		// should be powers of 2
		_width = (data[0] << 2) + data[1];
		_height = (data[2] << 2) + data[3];
		uint32_t size = ((uint32_t)_width * (uint32_t)_height) * 4;
		_pixels = (uint8_t*)memalign(32, size);
		memcpy(_pixels, data + 4);
	}

	uint8_t* getPixels() {
		return _pixels;
	}

	void updateTexture(bool repeat = false, bool antialias = false) {
		DCFlushRange(_pixels, ((uint32_t)_width * (uint32_t)_height) << 2);
		if (repeat) {
			GX_InitTexObj(&_texture, _pixels, _width, _height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
		}
		else
		{
			GX_InitTexObj(&_texture, _pixels, _width, _height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
		}

		GXRModeObj* mode = RenderGlobal::mode;

		if (antialias) {
			GX_SetCopyFilter(mode->aa, mode->sample_pattern, GX_TRUE, mode->vfilter);
		}
		else
		{
			GX_InitTexObjLOD(&_texture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
			GX_SetCopyFilter(GX_FALSE, mode->sample_pattern, GX_FALSE, mode->vfilter);
		}

		GX_InvalidateTexAll();
	}

private:
	uint16_t _width;
	uint16_t _height;
	uint8_t* _pixels;
	GXTexObj _texture;
};
#endif // !TEXTURE_H_
