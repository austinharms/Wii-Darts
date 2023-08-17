#ifndef DARTS_TEXTURE_HANDLE_H_
#define DARTS_TEXTURE_HANDLE_H_
#include "Core.h"
#include <ogc/gx.h>

enum WDPixelFormatEnum
{
	WD_PIXEL_RGBA8 = 0,
	WD_PIXEL_ARGB4X4
};

class TextureHandle
{
public:
	TextureHandle(const void* pixels, uint16_t width, uint16_t height, bool repeat = false, bool antialias = false, WDPixelFormatEnum srcFormat = WD_PIXEL_ARGB4X4);
	~TextureHandle();
	bool Bind(uint8_t slot = 0);

private:
	GXTexObj m_textureHandle;

	static void ConvertRGBA8PixelData(uint8_t* dst, uint32_t* src, uint16_t width, uint16_t height);
};
#endif // !DARTS_TEXTURE_HANDLE_H_
