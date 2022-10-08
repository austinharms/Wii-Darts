#include "Prefabs.h"

#include <stdint.h>
#include <new>

#include "core/BasicFontAtlas.h"
#include "font_256_RGBA8.h"

namespace wiidarts {
	namespace Prefabs {
		Mesh* DartMesh = new(std::nothrow) Mesh("./assets/dart.mesh", true);
		Mesh* DartBoardMesh = new(std::nothrow) Mesh("./assets/dart_board.mesh", true);
		Mesh* WallMesh = new(std::nothrow) Mesh("./assets/wall.mesh", true);
		Texture* DartTexture = new(std::nothrow) Texture("./assets/dart.RGBA8", false, true, true);
		Texture* DartBoardTexture = new(std::nothrow) Texture("./assets/dart_board.RGBA8", false, true, true);
		Texture* WallTexture = new(std::nothrow) Texture("./assets/wall.RGBA8", false, true, true);
		Texture* PackedFontTexture = new(std::nothrow) Texture(font_256_RGBA8, false, true, true);
		Texture* LargeFontTexture = new(std::nothrow) Texture((const char*)"./assets/font_1024.RGBA8", false, true, true);
		FontAtlas* DefaultFontAtlas = new(std::nothrow) BasicFontAtlas(LargeFontTexture && LargeFontTexture->getValid()?LargeFontTexture:PackedFontTexture, 10, 10, ' ', '~', '?');
	}
}