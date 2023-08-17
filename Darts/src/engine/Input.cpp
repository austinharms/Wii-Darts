#include "engine/Input.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

Input::Input() {
	m_init = false;
}

void Input::Init()
{
	uint16_t w, h;
	if (WPAD_Init() != WPAD_ERR_NONE) {
		Engine::Log("Failed to Init WPAD");
		Engine::Quit();
	}

	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetMotionPlus(WPAD_CHAN_ALL, true);
	Engine::GetRenderer().GetFramebufferSize(&w, &h);
	WPAD_SetVRes(WPAD_CHAN_ALL, w, h);
	Engine::Log("Input Init");
	m_init = true;
}

Input::~Input() {
	if (m_init) WPAD_Shutdown();
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
