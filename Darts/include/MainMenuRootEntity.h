#ifndef DARTS_MAIN_MENU_ROOT_ENTITY_H_
#define DARTS_MAIN_MENU_ROOT_ENTITY_H_
#include "Engine/RootEntity.h"
#include "TestRectEntity.h"

class MainMenuRoot : public RootEntity
{
public:
	MainMenuRoot() = default;
	~MainMenuRoot() = default;

protected:
	void OnLoad() WD_OVERRIDE {
		AddChild<TestRectEntity>();
	}
};
#endif // !DARTS_MAIN_MENU_ROOT_ENTITY_H_
