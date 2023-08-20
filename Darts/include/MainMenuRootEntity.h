#ifndef DARTS_MAIN_MENU_ROOT_ENTITY_H_
#define DARTS_MAIN_MENU_ROOT_ENTITY_H_
#include "engine/RootEntity.h"
#include "TestRectEntity.h"
#include "DartEntity.h"

class MainMenuRoot : public RootEntity
{
public:
	MainMenuRoot() = default;
	~MainMenuRoot() = default;

protected:
	void OnLoad() WD_OVERRIDE {
		AddChild<TestRectEntity>();
		AddChild<DartEntity>();
	}
};
#endif // !DARTS_MAIN_MENU_ROOT_ENTITY_H_
