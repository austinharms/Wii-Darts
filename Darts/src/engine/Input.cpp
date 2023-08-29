#include "engine/Input.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <ogc/gu.h>

Input::Input() {
	m_init = false;
	m_activeController = 0;
	m_inputDisabled = false;
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
	SetActiveController(0);
	EnableInputs();
	Engine::Log("Input Init");
	m_init = true;
}

void Input::EnableInputs()
{
	m_inputDisabled = false;
}

void Input::DisableInputs()
{
	m_inputDisabled = true;
}

Input::~Input() {
	if (m_init) WPAD_Shutdown();
}

bool Input::GetIRScreenPose(float* x, float* y)
{
	return GetIRScreenPose(m_activeController, x, y);
}

bool Input::GetButtonDown(uint32_t button)
{
	return GetButtonDown(m_activeController, button);
}

bool Input::GetButtonReleased(uint32_t button)
{
	return GetButtonReleased(m_activeController, button);
}

bool Input::GetButtonPressed(uint32_t button)
{
	return GetButtonPressed(m_activeController, button);
}

bool Input::GetOrientation(guVector& out)
{
	return GetOrientation(m_activeController, out);
}

bool Input::GetAccel(guVector& out)
{
	return GetAccel(m_activeController, out);;
}

bool Input::GetIRScreenPose(uint8_t controllerNumber, float* x, float* y)
{
	if (m_inputDisabled) return false;
	WPADData* data = WPAD_Data(controllerNumber);
	if (!data || !data->ir.raw_valid) return false;
	if (x) *x = data->ir.x;
	if (y) *y = data->ir.y;
	return true;
}

bool Input::GetButtonDown(uint8_t controllerNumber, uint32_t button)
{
	if (m_inputDisabled) return false;
	return WPAD_ButtonsHeld(controllerNumber) & button;;
}

bool Input::GetButtonReleased(uint8_t controllerNumber, uint32_t button)
{
	if (m_inputDisabled) return false;
	return WPAD_ButtonsUp(controllerNumber) & button;;
}

bool Input::GetButtonPressed(uint8_t controllerNumber, uint32_t button)
{
	if (m_inputDisabled) return false;
	return WPAD_ButtonsDown(controllerNumber) & button;
}

bool Input::GetOrientation(uint8_t ctrlNo, guVector& out)
{
	orient_t ori;
	WPAD_Orientation(ctrlNo, &ori);
	out.x = ori.pitch;
	out.y = ori.yaw;
	out.z = ori.roll;
	return true;
}

bool Input::GetAccel(uint8_t ctrlNo, guVector& out)
{
	gforce_t gf;
	WPAD_GForce(ctrlNo, &gf);
	out.x = gf.x;
	out.y = gf.y;
	out.z = gf.z;
	return true;
}

void Input::SetActiveController(uint8_t ctrlNo)
{
	m_activeController = ctrlNo;
}

WD_NODISCARD uint8_t Input::GetActiveController() const
{
	return m_activeController;
}

void Input::PollEvents() {
	WPAD_ScanPads();
}

/*
void WPAD_Orientation(int chan, struct orient_t *orient);
void WPAD_GForce(int chan, struct gforce_t *gforce);
void WPAD_Accel(int chan, struct vec3w_t *accel);
*/
