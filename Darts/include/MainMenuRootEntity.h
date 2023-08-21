#ifndef DARTS_MAIN_MENU_ROOT_ENTITY_H_
#define DARTS_MAIN_MENU_ROOT_ENTITY_H_
#include "engine/RootEntity.h"
#include "DartEntity.h"
#include "RoomEntity.h"
#include "engine/GUI.h"

class MainMenuRoot : public RootEntity
{
public:
	MainMenuRoot() = default;
	~MainMenuRoot() = default;

protected:
	void OnLoad() WD_OVERRIDE {
		AddChild<RoomEntity>();
		AddChild<DartEntity>();
	}

	void OnRender() {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(io.DisplaySize);
		//ImGui::ShowDemoWindow();
	}
};
#endif // !DARTS_MAIN_MENU_ROOT_ENTITY_H_
