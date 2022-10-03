#include "Texture.h"

#include <ogc/gx.h>
#include <ogc/cache.h>
#include <malloc.h>
#include <cstring>

#include "Renderer.h"

#define is_aligned(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

namespace wiidarts {
	Texture::Texture(uint16_t width, uint16_t height, const uint32_t* rgbaPixelBuffer, bool repeat, bool antialias)
	{
		_locked = false;
		_dirty = true;
		_repeat = repeat;
		_antialias = antialias;
		_width = width;
		_height = height;
		_allocatedBuffer = true;
		uint32_t size = ((uint32_t)_width * (uint32_t)_height) << 2;
		_pixelBuffer = (uint8_t*)memalign(32, size);
		if (!_pixelBuffer) return;
		memset(_pixelBuffer, 0x00, size);
		if (rgbaPixelBuffer) {
			uint32_t index = 0;
			for (int x = 0; x < _width; ++x) {
				for (int y = 0; y < _height; ++y) {
					setPixelRGBA(x, y, _pixelBuffer[index++]);
				}
			}
		}
	}

	Texture::Texture(const uint8_t* pixelBuffer, bool repeat, bool antialias, bool locked)
	{
		_dirty = true;
		_locked = false;
		_repeat = repeat;
		_antialias = antialias;
		_width = ((uint16_t*)pixelBuffer)[0];
		_height = ((uint16_t*)pixelBuffer)[1];
		// pad after the size bytes to allow for 32 byte alignment on pixel bytes
		_pixelBuffer = (uint8_t*)pixelBuffer + 32;
		if (locked && is_aligned(_pixelBuffer, 32)) {
			_allocatedBuffer = false;
			lock();
			return;
		}

		_allocatedBuffer = true;
		uint32_t size = ((uint32_t)_width * (uint32_t)_height) << 2;
		_pixelBuffer = (uint8_t*)memalign(32, size);
		if (!_pixelBuffer) return;
		memcpy(_pixelBuffer, pixelBuffer + (sizeof(uint16_t) * 2), size);
		if (locked) lock();
	}

	Texture::Texture(const Texture& other)
	{
		_dirty = true;
		_locked = false;
		_repeat = other._repeat;
		_antialias = other._antialias;
		_width = other._width;
		_height = other._height;
		_allocatedBuffer = true;
		uint32_t size = ((uint32_t)_width * (uint32_t)_height) << 2;
		_pixelBuffer = (uint8_t*)memalign(32, size);
		if (!_pixelBuffer) return;
		memcpy(_pixelBuffer, other._pixelBuffer, size);
	}

	Texture::~Texture()
	{
		if (_allocatedBuffer && _pixelBuffer)
			free(_pixelBuffer);
		_pixelBuffer = nullptr;
	}

	void Texture::bind(uint8_t slot)
	{
		//if (_dirty) updateGXTexture();
		updateGXTexture();
		GX_LoadTexObj(&_gxTexture, GX_TEXMAP0 + slot);
	}

	void Texture::lock()
	{
		_locked = true;
		updateGXTexture();
	}

	bool Texture::getLocked() const
	{
		return _locked;
	}

	bool Texture::getAntialias() const
	{
		return _antialias;
	}

	bool Texture::getRepeat() const
	{
		return _repeat;
	}

	bool Texture::getValid() const
	{
		return _pixelBuffer != nullptr;
	}

	uint16_t Texture::getHeight() const
	{
		return _height;
	}

	uint16_t Texture::getWidth() const
	{
		return _width;
	}

	void Texture::setPixelRGBA(const int x, const int y, const uint32_t color) {
		if (_locked) return;
		uint32_t  offs;
		offs = (((y & (~3)) << 2) * _width) + ((x & (~3)) << 4) + ((((y & 3) << 2) + (x & 3)) << 1);
		*((uint16_t*)(_pixelBuffer + offs)) = (uint16_t)((color << 8) | (color >> 24));
		*((uint16_t*)(_pixelBuffer + offs + 32)) = (uint16_t)(color >> 8);
		_dirty = true;
	}

	void Texture::setAntialias(bool value)
	{
		if (_locked) return;
		_antialias = value;
		_dirty = true;
	}

	void Texture::setRepeat(bool value)
	{
		if (_locked) return;
		_repeat = value;
		_dirty = true;
	}

	const uint8_t* Texture::getRawPixelBuffer() const
	{
		if (_locked) return nullptr;
		return _pixelBuffer;
	}

	void Texture::updateGXTexture()
	{
		_dirty = false;
		DCFlushRange(_pixelBuffer, ((uint32_t)_width * (uint32_t)_height) << 2);
		if (_repeat) {
			GX_InitTexObj(&_gxTexture, _pixelBuffer, _width, _height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
		}
		else
		{
			GX_InitTexObj(&_gxTexture, _pixelBuffer, _width, _height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
		}

		GXRModeObj* mode = Renderer::getInstance().getMode();
		if (_antialias) {
			GX_SetCopyFilter(mode->aa, mode->sample_pattern, GX_TRUE, mode->vfilter);
		}
		else
		{
			GX_InitTexObjLOD(&_gxTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
			GX_SetCopyFilter(GX_FALSE, mode->sample_pattern, GX_FALSE, mode->vfilter);
		}

		//GX_InvalidateTexAll();
	}
}