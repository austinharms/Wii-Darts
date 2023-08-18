#include "engine/Engine.h"
#include "MainMenuRootEntity.h"

int main(int argc, char** argv) {
	Engine::SetRootEntity<MainMenuRoot>();
	Engine::Start(argc, argv);
	//Engine::Quit();
	return 0;
}