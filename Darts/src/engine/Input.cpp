#include "engine/Input.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

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

bool Input::GetControllerIRScreenPos(uint8_t controllerNumber, float* x, float* y)
{
	WPADData* data = WPAD_Data(controllerNumber);
	if (!data || !data->ir.raw_valid) return false;
	if (x) *x = data->ir.x;
	if (y) *y = data->ir.y;
	return true;
}

bool Input::GetControllerButtonDown(uint8_t controllerNumber, uint32_t button)
{
	return WPAD_ButtonsHeld(controllerNumber) & button;;
}

bool Input::GetControllerButtonPressed(uint8_t controllerNumber, uint32_t button)
{
	return WPAD_ButtonsDown(controllerNumber) & button;;
}

bool Input::GetControllerButtonReleased(uint8_t controllerNumber, uint32_t button)
{
	return WPAD_ButtonsUp(controllerNumber) & button;
}

void Input::PollEvents() {
	WPAD_ScanPads();
}
