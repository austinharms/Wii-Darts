#ifndef DARTS_TEXTURE_HANDLE_H_
#define DARTS_TEXTURE_HANDLE_H_
#include "Core.h"
#include <ogc/gx.h>

// Handle for texture data stored in memory
// You are responsible for the lifetime of the pixel buffer and that is it the correct format (ARGB4X4) and alignment (32byte)
class TextureHandle
{
public:
	// Converts pixel data format from RGBA8 to ARGB4X4
	// The dst and src buffers must not overlap
	static void ConvertRGBA8PixelData(uint8_t* dst, uint32_t* src, uint16_t width, uint16_t height);

	TextureHandle();
	~TextureHandle();
	WD_NOCOPY(TextureHandle);

	bool Init(void* pixels, uint16_t width, uint16_t height, bool repeat = false, bool antialias = false);
	WD_NODISCARD bool GetValid();
	bool Bind(uint8_t slot = 0);

private:
	GXTexObj m_textureHandle;


};
#endif // !DARTS_TEXTURE_HANDLE_H_
