#include <stdint.h>

#include "FontAtlas.h"
#include "Texture.h"

#ifndef WIIDARTS_BASICFONTATLAS_H_
#define WIIDARTS_BASICFONTATLAS_H_
namespace wiidarts {
	class BasicFontAtlas : public FontAtlas
	{
	public:
		BasicFontAtlas(Texture* fontTexture, uint8_t charGridWidth, uint8_t charGridHeight, char firstChar, char lastChar, char errorChar) {
			_fontTexture = fontTexture;
			_charGridWidth = charGridWidth;
			_charGridHeight = charGridHeight;
			_firstChar = firstChar;
			_lastChar = lastChar;
			_errorChar = errorChar;
			if (_fontTexture != nullptr)
				_fontTexture->grab();
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
			return _errorChar;
		}

		bool hasChar(char letter) const override {
			return letter >= _firstChar && letter <= _lastChar;
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
			
			// letter is treated and a the index of the letter on the texture
			letter -= _firstChar;
			letter = (_charGridHeight * _charGridWidth - 1) - letter;
			float charUVWidth = 1.0f / (float)_charGridWidth;
			float charUVHeight = 1.0f / (float)_charGridHeight;
			buf[3] = (letter % _charGridWidth) * charUVWidth;
			buf[4] = (letter / _charGridWidth) * charUVHeight;
			buf[8] = buf[3] + charUVWidth;
			buf[9] = buf[4];
			buf[13] = buf[8];
			buf[19] = buf[4] + charUVHeight;
			buf[18] = buf[3];
			buf[14] = buf[19];
		}

	private:
		char _firstChar;
		char _lastChar;
		char _errorChar;
		uint8_t _charGridWidth;
		uint8_t _charGridHeight;
		Texture* _fontTexture;

		BasicFontAtlas(const BasicFontAtlas&);
		void operator=(const BasicFontAtlas&);
	};
}
#endif // !WIIDARTS_BASICFONTATLAS_H_
