#ifndef DARTS_MAIN_MENU_ROOT_ENTITY_H_
#define DARTS_MAIN_MENU_ROOT_ENTITY_H_
#include "engine/Core.h"
#include "engine/RootEntity.h"

class MainMenuRoot : public RootEntity
{
public:
	MainMenuRoot();
	virtual ~MainMenuRoot();

protected:
	void OnLoad() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;

private:
	struct SceneData;
	SceneData& GetData();
	const SceneData& GetData() const;
};
#endif // !DARTS_MAIN_MENU_ROOT_ENTITY_H_
