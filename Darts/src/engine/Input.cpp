#include "engine/Input.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <wiiuse/wpad.h>

Input::Input() {
	uint16_t w = 0;
	uint16_t h = 0;
	Engine::GetRenderer().GetFramebufferSize(&w, &h);
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, w, h);
}

Input::~Input() {

}

void Input::PollEvents() {
	WPAD_ScanPads();
}
