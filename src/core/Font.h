#include "RefCount.h"
#include "Renderer.h"
#include "Mesh.h"
#include "FontAtlas.h"

#ifndef WIIDARTS_FONT_H_
#define WIIDARTS_FONT_H_
namespace wiidarts {
	class Font : public RefCount
	{
	public:
		const static uint16_t CHAR_MESH_VERTEX_COUNT;
		const static uint16_t CHAR_MESH_INDEX_COUNT;
		const static float CHAR_MESH_VERTEX_BUFFER[];
		const static uint16_t CHAR_MESH_INDEX_BUFFER[];

		Font(FontAtlas* atlas = nullptr);
		virtual ~Font();
		bool getValid() const;
		// in screen coordinates (bottom right is 0,0)
		void setCursor(float x = 0, float y = 0);
		void setColor(uint32_t color);
		uint32_t getColor() const;
		void setSize(float size);
		float getSize() const;
		void setFontAtlas(FontAtlas* atlas);
		FontAtlas* getFontAtlas() const;
		void drawText(const char* text);

	private:

		FontAtlas* _fontAtlas;
		Mesh* _charMesh;
		Transform _transform;
		uint32_t _color;
		float _size;
		float _left;

		void drawChar(char letter);
	};
}
#endif // !WIIDARTS_FONT_H_
