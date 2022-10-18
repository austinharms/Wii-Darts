#include "../core/Entity.h"

#ifndef WIIDARTS_SCENE_H_
#define WIIDARTS_SCENE_H_
namespace wiidarts {
	namespace Scene {
		enum Scene
		{
			NONE = 0,
			DART_BOARD,
			BRICK_ROOM
		};

		bool LoadScene(Scene scene);
		extern Entity* _loadedSceneParent;
	}
}
#endif // !WIIDARTS_SCENE_H_