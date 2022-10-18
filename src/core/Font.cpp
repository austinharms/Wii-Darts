#include "Font.h"

#include <ogc/gu.h>
#include <new>

#include "Renderer.h"

namespace wiidarts {
	const uint16_t Font::CHAR_MESH_VERTEX_COUNT = 4;
	const uint16_t Font::CHAR_MESH_INDEX_COUNT = 6;
	const float Font::CHAR_MESH_VERTEX_BUFFER[CHAR_MESH_VERTEX_COUNT * Mesh::FLOATS_PER_VERTEX] = {
		0, 0, 0, 0, 0,
		1, 0, 0, 1, 0,
		1, 1, 0, 1, 1,
		0, 1, 0, 0, 1
	};
	const uint16_t Font::CHAR_MESH_INDEX_BUFFER[CHAR_MESH_INDEX_COUNT] = {
		0, 1, 2,
		0, 2, 3
	};

	Font::Font(FontAtlas* atlas)
	{
		_size = 1;
		_left = 0;
		_color = 0xffffffff;
		_fontAtlas = nullptr;
		setFontAtlas(atlas);
		_charMesh = new(std::nothrow) Mesh(CHAR_MESH_VERTEX_COUNT, CHAR_MESH_INDEX_COUNT, CHAR_MESH_INDEX_BUFFER, CHAR_MESH_VERTEX_BUFFER);
	}

	Font::~Font()
	{
		setFontAtlas(nullptr);
		if (_charMesh != nullptr)
			_charMesh->drop();
		_charMesh = nullptr;
	}

	bool Font::getValid() const
	{
		return _charMesh && _charMesh->getValid() && _fontAtlas && _fontAtlas->getValid();
	}

	void Font::setCursor(float x, float y)
	{
		_transform.setPosition({ x, y, 0 });
		_left = x;
	}

	void Font::setColor(uint32_t color)
	{
		_color = color;
	}

	uint32_t Font::getColor() const
	{
		return _color;
	}

	void Font::setSize(float size)
	{
		float scale = 1.0f / _size * size;
		_transform.scale({ scale, scale, scale });
		_size = size;
	}

	float Font::getSize() const
	{
		return _size;
	}

	void Font::setFontAtlas(FontAtlas* atlas)
	{
		if (_fontAtlas != nullptr) _fontAtlas->drop();
		_fontAtlas = atlas;
		if (_fontAtlas) _fontAtlas->grab();
	}

	FontAtlas* Font::getFontAtlas() const
	{
		return _fontAtlas;
	}

	void Font::drawText(const char* text)
	{
		if (!getValid()) return;
		_fontAtlas->bindTexture(0);
		while (text[0])
		{
			drawChar(text[0]);
			text += 1;
		}
	}

	// this currently assumes square letters
	// TODO update this to use getCharWidth and getCharHeight to resize charMesh
	void Font::drawChar(char letter)
	{
		if (letter == '\n') {
			guVector pos = _transform.getPosition();
			pos.y -= _fontAtlas->getMaxCharHeight() * _size;
			pos.x = _left;
			_transform.setPosition(pos);
		}
		else {
			if (!_fontAtlas->hasChar(letter))
				letter = _fontAtlas->getErrorChar();
			_fontAtlas->setMeshCharUV(letter, *_charMesh);
			Renderer::getInstance().drawMesh2D(*_charMesh, _transform, _color);
			_transform.translate({ _fontAtlas->getCharWidth(letter) * _size, 0, 0 });
		}
	}
}