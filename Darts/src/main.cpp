#include "Engine/Engine.h"
#include "MainMenuRootEntity.h"

int main() {
	Engine::SetRootEntity<MainMenuRoot>();
	Engine::Start();
	return 0;
}