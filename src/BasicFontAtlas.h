#include <stdint.h>

#include "core/FontAtlas.h"
#include "core/Texture.h"
#include "font_512_RGBA8.h"

#ifndef WIIDARTS_BASICFONTATLAS_H_
#define WIIDARTS_BASICFONTATLAS_H_
namespace wiidarts {
	class BasicFontAtlas : public FontAtlas
	{
	public:
		static BasicFontAtlas& getInstance()
		{
			static BasicFontAtlas instance;
			return instance;
		}

		virtual ~BasicFontAtlas() {
			if (_fontTexture)
				_fontTexture->drop();
			_fontTexture = nullptr;
		}

		bool getValid() const override {
			return _fontTexture && _fontTexture->getValid();
		}

		char getErrorChar() const override {
			return ' ';
		}

		bool hasChar(char letter) const override {
			return letter >= 32 && letter < 127;
		}

		float getCharWidth(char letter) const override {
			switch (letter)
			{
			case 'W':
			case 'M':
			case '@':
				return 1;
			case '!':
			case '1':
			case 'i':
			case 'I':
			case 'j':
			case 't':
			case 'l':
			case ' ':
				return 0.55f;
			default:
				return 0.65f;
			}
		}

		float getCharHeight(char letter) const override {
			return 1;
		}

		float getMaxCharHeight() const override {
			return 1;
		}

		void bindTexture(uint8_t slot) override {
			if (_fontTexture)
				_fontTexture->bind(slot);
		}

		void setMeshCharUV(char letter, Mesh& charMesh) const override {
			float* buf = (float*)charMesh.getVertexBufferPtr();
			//this is crappy but works for now
			//TODO refactor this
			letter = -letter + 131;
			constexpr float charUVSize = 1.0f / 10.0f;
			buf[3] = (letter % 10) * charUVSize;
			buf[4] = (letter / 10) * charUVSize;
			buf[8] = buf[3] + charUVSize;
			buf[9] = buf[4];
			buf[13] = buf[8];
			buf[19] = buf[4] + charUVSize;
			buf[18] = buf[3];
			buf[14] = buf[19];
		}

	private:
		Texture* _fontTexture;

		BasicFontAtlas() {
			_fontTexture = new(std::nothrow) Texture(font_512_RGBA8, false, true, true);
		}

		BasicFontAtlas(const BasicFontAtlas&);
		void operator=(const BasicFontAtlas&);
	};
}
#endif // !WIIDARTS_BASICFONTATLAS_H_
