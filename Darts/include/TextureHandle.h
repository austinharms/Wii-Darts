#ifndef DARTS_TEXTURE_HANDLE_H_
#define DARTS_TEXTURE_HANDLE_H_
#include "Core.h"
#include <ogc/gx.h>

class TextureHandle
{
public:
	TextureHandle(void* pixels, uint32_t width, uint32_t height, bool repeat = false, bool antialias = false);
	~TextureHandle();
	bool Bind(uint8_t slot = 0);

private:
	GXTexObj m_textureHandle;
};
#endif // !DARTS_TEXTURE_HANDLE_H_
