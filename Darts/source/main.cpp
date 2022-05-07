#include <stdint.h>
#include <stdlib.h>
#include <cstdio>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "rendering/RenderMeshFactory.h"
#include "rendering/Renderer.h"
#include "entitys/Dart.h"
#include "entitys/RenderEntity.h"
#include "rendering/RenderGlobal.h"
#include "Avgf.h"

Entity* masterEntity = nullptr;

// Creates back wall and dart board
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
	Renderer* renderer = &Renderer::getInstance();
	renderer->setLookAndPosition(Vector3f(0, 1.72f, 0), Vector3f(0, 1.72f, -2.37f));

	// Init Controller System  
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	// Must be done after Renderer initialization because of fbWidth and efbHeight
	WPAD_SetVRes(WPAD_CHAN_ALL, RenderGlobal::mode->fbWidth, RenderGlobal::mode->efbHeight);
	
	// Create the "Master" Entity, this is the top level parent node that is updated every frame
	masterEntity = new Entity();
	createRoom();

	// Add a temp Dart entity
	Dart* dart = new Dart();
	Avgf dartX(5, 0);
	Avgf dartY(5, 0);
	dart->setPosition(Vector3f(0, 1.72f, -1));
	masterEntity->addChild((Entity*)dart);

	while (true) {
		// Update Controller Buffers
		WPAD_ScanPads();
		WPADData* wiimote0 = WPAD_Data(0);
		if (wiimote0->btns_d & WPAD_BUTTON_HOME) break;

		// Rumble Wiimote 0 if b button held down
		WPAD_Rumble(WPAD_CHAN_0, (wiimote0->btns_h & WPAD_BUTTON_B) ? 1 : 0);

		// Point the dart based on IR position
		float xPos = wiimote0->ir.x / (float)RenderGlobal::mode->fbWidth;
		float yPos = wiimote0->ir.y / (float)RenderGlobal::mode->efbHeight;
		if (xPos != 0)
			dartX.add(-(xPos * 60 - 30));
		if (yPos != 0)
			dartY.add(-(yPos * 60 - 30));
		dart->setRotation(Vector3f(dartY,dartX,0));

		// Update/render all the entities
		masterEntity->update();

		// Draw Debug Text
		Vector3f textPos(-1.5f, 2.65f, -2);
		char strBuf[256];
		sprintf(accelStr, "IR: %+09.6f/%+i, %+09.6f/%+i", wiimote0->ir.x, RenderGlobal::mode->fbWidth, wiimote0->ir.y, RenderGlobal::mode->efbHeight);
		renderer->drawString(accelStr, textPos, 0.1f, 0xffffffff);
		textPos.x = -1.5f;
		textPos.y -= 0.15f;
		sprintf(strBuf, "Bat: %u", wiimote0->battery_level);
		renderer->drawString(accelStr, textPos, 0.1f, 0xffffffff);

		// Show/Swap framebuffer and wait for next frame
		renderer->swapFrameBuffer();
	}

	dart->drop();
	masterEntity->drop();
	masterEntity = nullptr;
	return 0;
}
