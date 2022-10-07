#include "Prefabs.h"

#include <stdint.h>
#include <new>

#include "core/BasicFontAtlas.h"
#include "dart_mesh.h"
#include "dart_board_mesh.h"
#include "wall_mesh.h"
#include "wall_RGBA8.h"
#include "dart_board_small_RGBA8.h"
#include "dart_RGBA8.h"
#include "font_1024_RGBA8.h"

namespace wiidarts {
	namespace Prefabs {
		Mesh* DartMesh = new(std::nothrow) Mesh(dart_mesh);
		Mesh* DartBoardMesh = new(std::nothrow) Mesh(dart_board_mesh);
		Mesh* WallMesh = new(std::nothrow) Mesh(wall_mesh);
		Texture* DartTexture = new(std::nothrow) Texture(dart_RGBA8, false, true, true);
		Texture* DartBoardTexture = new(std::nothrow) Texture(dart_board_small_RGBA8, false, true, true);
		Texture* WallTexture = new(std::nothrow) Texture(wall_RGBA8, false, true, true);
		Texture* LargeFontTexture = new(std::nothrow) Texture(font_1024_RGBA8, false, true, true);
		FontAtlas* DefaultFontAtlas = new(std::nothrow) BasicFontAtlas(LargeFontTexture, 10, 10, ' ', '~', '?');
	}
}