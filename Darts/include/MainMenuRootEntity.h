#ifndef DARTS_MAIN_MENU_ROOT_ENTITY_H_
#define DARTS_MAIN_MENU_ROOT_ENTITY_H_
#include "engine/RootEntity.h"
#include "DartEntity.h"
#include "RoomEntity.h"
#include "CameraEntity.h"
#include "engine/LightEntity.h"

class MainMenuRoot : public RootEntity
{
public:
	MainMenuRoot() = default;
	virtual ~MainMenuRoot() = default;

protected:
	void OnLoad() WD_OVERRIDE {
		AddChild<LightEntity>();
		AddChild<RoomEntity>();
		AddChild<DartEntity>();
		AddChild<CameraEntity>();
	}
};
#endif // !DARTS_MAIN_MENU_ROOT_ENTITY_H_
