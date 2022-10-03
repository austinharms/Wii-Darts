#include <stdint.h>
#include <ogc/gx.h>

#include "RefCount.h"

#ifndef WIIDARTS_TEXTURE_H_
#define WIIDARTS_TEXTURE_H_
namespace wiidarts {
	class Texture : public RefCount
	{
	public:
		Texture(uint16_t width, uint16_t height, const uint32_t* rgbaPixelBuffer = nullptr, bool repeat = false, bool antialias = false);
		Texture(const uint8_t* pixelBuffer, bool repeat = false, bool antialias = false, bool locked = false);
		Texture(const Texture& other);
		virtual ~Texture();
		void bind(uint8_t slot = 0);
		void lock();
		bool getLocked() const;
		bool getAntialias() const;
		bool getRepeat() const;
		bool getValid() const;
		uint16_t getHeight() const;
		uint16_t getWidth() const;
		// bottom right is 0,0
		void setPixelRGBA(const int x, const int y, const uint32_t color);
		void setAntialias(bool value);
		void setRepeat(bool value);
		// will return nullptr if the texture is locked
		const uint8_t* getRawPixelBuffer() const;

	private:
		// Pixel Buffer is RGBA32/RGBA8 https://wiki.tockdom.com/wiki/Image_Formats 
		uint8_t* _pixelBuffer;
		GXTexObj _gxTexture;
		uint16_t _width;
		uint16_t _height;
		bool _locked;
		bool _allocatedBuffer;
		bool _repeat;
		bool _antialias;
		bool _dirty;

		void updateGXTexture();
		Texture& operator=(const Texture& other);
	};
}
#endif // !WIIDARTS_TEXTURE_H_
