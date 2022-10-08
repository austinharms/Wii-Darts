#include "core/Mesh.h"
#include "core/Texture.h"
#include "core/FontAtlas.h"

#ifndef WIIDARTS_PREFABS_H_
#define WIIDARTS_PREFABS_H_
namespace wiidarts {
	namespace Prefabs {
		extern Mesh* DartMesh;
		extern Mesh* DartBoardMesh;
		extern Mesh* WallMesh;
		extern Texture* DartTexture;
		extern Texture* DartBoardTexture;
		extern Texture* WallTexture;
		extern Texture* LargeFontTexture;
		extern Texture* PackedFontTexture;
		extern FontAtlas* DefaultFontAtlas;
	}
}
#endif // !WIIDARTS_PREFABS_H_
