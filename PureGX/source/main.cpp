#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "rendering/RenderMeshFactory.h"
#include "rendering/Renderer.h"
#include "entitys/TestEntity.h"
#include "entitys/RenderEntity.h"

Entity* masterEntity = nullptr;

void createRoom() {
	Entity* en = new RenderEntity(RM3D_WALL);
	en->setPosition(Vector3f(0, 1.72f, -2.371f));
	masterEntity->addChild(en);
	en->drop();
	en = new RenderEntity(RM3D_BOARD);
	en->scale(Vector3f(0.4572f));
	en->setPosition(Vector3f(0, 1.72f, -2.37f));
	en->rotate(Vector3f(-90, 0, 180));
	masterEntity->addChild(en);
	en->drop();
	en = nullptr;
}

int main(void) {
	WPAD_Init();
	Renderer::getInstance().setLookAndPosition(Vector3f(0, 1.72f, 0), Vector3f(0, 1.72f, -2.37f));
	masterEntity = new Entity();
	createRoom();

	{
		TestEntity* dart = new TestEntity();
		masterEntity->addChild((Entity*)dart);
		dart->drop();
		dart = nullptr;
	}

	while (true) {
		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
		masterEntity->update();
		Renderer* r = &Renderer::getInstance();
		r->swapFrameBuffer();
	}

	masterEntity->drop();
	masterEntity = nullptr;
	return 0;
}
