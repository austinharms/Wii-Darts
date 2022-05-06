#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "rendering/RenderMeshFactory.h"
#include "rendering/Renderer.h"
#include "entitys/Dart.h"
#include "entitys/RenderEntity.h"
#include "rendering/RenderGlobal.h"
#include <stdio.h>

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
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC);
	Renderer::getInstance().setLookAndPosition(Vector3f(0, 1.72f, 0), Vector3f(0, 1.72f, -2.37f));
	masterEntity = new Entity();
	createRoom();

	Dart* dart = new Dart();
	dart->setPosition(Vector3f(0, 1.72f, -1));
	masterEntity->addChild((Entity*)dart);

	while (true) {
		WPAD_ScanPads();
		WPAD_SetVRes(WPAD_CHAN_ALL, RenderGlobal::mode->fbWidth, RenderGlobal::mode->efbHeight);
		WPADData* wiimote0 = WPAD_Data(0);
		if (wiimote0->btns_d & WPAD_BUTTON_HOME) break;
		WPAD_Rumble(WPAD_CHAN_0, (wiimote0->btns_d & WPAD_BUTTON_B) ? 1 : 0);
		dart->setRotation(Vector3f(wiimote0->gforce.x * 70,0,0));
		masterEntity->update();
		Renderer* r = &Renderer::getInstance();
		Vector3f textPos(-1.5f, 2.65f, -2);
		char accelStr[256];
		sprintf(accelStr, "Accel: %04u, %04u, %04u", wiimote0->accel.x, wiimote0->accel.y, wiimote0->accel.z);
		r->drawString(accelStr, textPos, 0.1f, 0xffffffff);
		textPos.x = -1.5f;
		textPos.y -= 0.15f;
		sprintf(accelStr, "G: %'+09.6f, %'+09.6f, %'+09.6f", wiimote0->gforce.x, wiimote0->gforce.y, wiimote0->gforce.z);
		r->drawString(accelStr, textPos, 0.1f, 0xffffffff);
		textPos.x = -1.5f;
		textPos.y -= 0.15f;
		sprintf(accelStr, "Bat: %u", wiimote0->battery_level);
		r->drawString(accelStr, textPos, 0.1f, 0xffffffff);
		textPos.x = -1.5f;
		textPos.y -= 0.15f;
		sprintf(accelStr, "Rot: %+09.6f, %+09.6f, %+09.6f", wiimote0->orient.pitch, wiimote0->orient.yaw, wiimote0->orient.roll);
		r->drawString(accelStr, textPos, 0.1f, 0xffffffff);
		r->swapFrameBuffer();
	}

	dart->drop();
	masterEntity->drop();
	masterEntity = nullptr;
	return 0;
}
