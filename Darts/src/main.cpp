#include "EngineCore.h"
#include "MainMenuRootEntity.h"

int main() {
	EngineCore::SetRootEntity<MainMenuRoot>();
	EngineCore::Start();
	return 0;
}