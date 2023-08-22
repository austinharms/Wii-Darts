#include "engine/Engine.h"
#include "MainMenuRootEntity.h"

int main(int argc, char** argv) {
	Engine::Create(argc, argv);
	Engine::SetRootEntity<MainMenuRoot>();
	Engine::Start();
	//Engine::Quit();
	return 0;
}