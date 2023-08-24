#ifndef DARTS_MAIN_MENU_ROOT_ENTITY_H_
#define DARTS_MAIN_MENU_ROOT_ENTITY_H_
#include "engine/RootEntity.h"
#include "DartEntity.h"
#include "MeshEntity.h"
#include "CameraEntity.h"
#include "engine/LightEntity.h"

class MainMenuRoot : public RootEntity
{
public:
	MainMenuRoot() = default;
	virtual ~MainMenuRoot() = default;

protected:
	void OnLoad() WD_OVERRIDE {
		AddChild<LightEntity>()->GetTransform().Translate({0,0,5});
		//AddChild<MeshEntity>("./assets/room.mesh", "./assets/room.img");
		AddChild<MeshEntity>("./assets/dense_plane.mesh");
		AddChild<DartEntity>();
		AddChild<CameraEntity>();
	}
};
#endif // !DARTS_MAIN_MENU_ROOT_ENTITY_H_
