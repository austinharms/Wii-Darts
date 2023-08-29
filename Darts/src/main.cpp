#include "engine/Engine.h"
#include "MainMenuRoot.h"

int main(int argc, char** argv) {
	Engine::Create(argc, argv);
	Engine::SetRootEntity<MainMenuRoot>();
	Engine::Start();
	return 0;
}