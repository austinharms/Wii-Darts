#include "EngineCore.h"
#include "MainMenuRootEntity.h"

int main() {
	darts::EngineCore::SetRootEntity<darts::MainMenuRoot>();
	darts::EngineCore::Start();
	return 0;
}