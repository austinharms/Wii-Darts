#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <grrlib.h>
#include <stdint.h>

class Texture
{
public:
	Texture(const void* data) {
		_grrTexture = GRRLIB_LoadTexture((const uint8_t*)data);
	}

	~Texture() {
		GRRLIB_FreeTexture(_grrTexture);
	}

	uint32_t getHeight() const {
		return _grrTexture->h;
	}

	uint32_t getWidth() const {
		return _grrTexture->w;
	}

	void bind() const {
		GRRLIB_SetTexture(_grrTexture, 0);
	}

private:
	GRRLIB_texImg* _grrTexture;
};

#endif // !TEXTURE_H_
