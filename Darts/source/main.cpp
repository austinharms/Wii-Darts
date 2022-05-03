#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "rendering/RenderMeshFactory.h"
#include "rendering/Renderer.h"
#include "entitys/TestEntity.h"
#include "entitys/RenderEntity.h"
#include "rendering/RenderGlobal.h"

Entity* masterEntity = nullptr;

void createRoom() {
	Entity* en = new RenderEntity(RM3D_WALL);
	en->setPosition(Vector3f(0, 1.72f, -2.371f));
	masterEntity->addChild(en);
	en->drop();
	en = new RenderEntity(RM3D_BOARD);
	en->scale(Vector3f(0.4572f));
	en->setPosition(Vector3f(0, 1.72f, -2.37f));
	en->rotate(Vector3f(90, 0, 0));
	masterEntity->addChild(en);
	en->drop();
	en = nullptr;
}

int main(void) {
	WPAD_Init();
	Renderer::getInstance().setLookAndPosition(Vector3f(0, 1.72f, 0), Vector3f(0, 1.72f, -2.37f));
	masterEntity = new Entity();
	createRoom();

	Entity* dart = new RenderEntity(RM3D_DART);
	dart->scale(Vector3f(0.02225f));
	dart->setPosition(Vector3f(0, 1.72f, -1));
	dart->rotate(Vector3f(0, 90, 0));
	masterEntity->addChild(dart);
	//dart->drop();
	uint32_t i = 0;
	while (true) {
		WPAD_ScanPads();
		WPAD_SetVRes(WPAD_CHAN_ALL, RenderGlobal::mode->fbWidth, RenderGlobal::mode->efbHeight);

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) break;
		masterEntity->update();
		Renderer* r = &Renderer::getInstance();
		r->swapFrameBuffer();

		if (++i == 1000) {
			masterEntity->removeChild(dart)->drop();
		}
	}

	masterEntity->drop();
	masterEntity = nullptr;
	return 0;
}
