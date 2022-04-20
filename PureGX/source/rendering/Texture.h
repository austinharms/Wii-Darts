#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <stdint.h>
#include <malloc.h>
#include <cstring>

#include "RenderGlobal.h"

class Texture
{
public:
	Texture(const void* data, bool repeat = false, bool antialias = false) {
		_pixels = nullptr;
		loadTexture((const uint32_t*)data);
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

	void loadTexture(const uint32_t* data) {
		if (_pixels != nullptr)
			free(_pixels);
		_width = 100;// data[0] >> 16;
		_height = 100;//(uint16_t)data[0];
		uint32_t size = ((uint32_t)_width * (uint32_t)_height) << 2;
		_pixels = (uint8_t*)memalign(32, size);
		memset(_pixels, 0x00, size);
		uint32_t index = 0;
		for (uint16_t w = 0; w < _width; ++w) {
			for (uint16_t h = 0; h < _height; ++h) {
				setPixel(w, h, data[++index]);
			}
		}
	}

	void  setPixel(const int x, const int y, const uint32_t color) {
		uint32_t  offs;
		offs = (((y & (~3)) << 2) * _width) + ((x & (~3)) << 4) + ((((y & 3) << 2) + (x & 3)) << 1);

		*((u16*)(_pixels + offs)) = (u16)((color << 8) | (color >> 24));
		*((u16*)(_pixels + offs + 32)) = (u16)(color >> 8);
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
