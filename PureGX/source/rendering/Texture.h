#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <stdint.h>

class Texture
{
public:
	Texture(const void* data, bool repeat = false, bool antialias = false);
	~Texture();
	uint16_t getHeight() const;
	uint16_t getWidth() const;
	void bind();

private:
	uint16_t _width;
	uint16_t _height;
	uint8_t* _pixels;
	GXTexObj _texture;
};
#endif // !TEXTURE_H_
