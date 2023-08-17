#ifndef DARTS_INPUT_H_
#define DARTS_INPUT_H_
#include "Core.h"
#include <wiiuse/wpad.h> // Include this in the header to allow WPAD_BUTTON_* to be visible in other headers

class Engine;

class Input
{
public:
	~Input();
	WD_NOCOPY(Input);
	bool GetControllerIRScreenPos(uint8_t controllerNumber, float* x, float* y);
	bool GetControllerButtonDown(uint8_t controllerNumber, uint32_t button);
	bool GetControllerButtonPressed(uint8_t controllerNumber, uint32_t button);
	bool GetControllerButtonReleased(uint8_t controllerNumber, uint32_t button);

private:
	friend class Engine;

	bool m_init;

	Input();

	void Init();
	// Updates WPAD inputs
	void PollEvents();
};
#endif // !DARTS_INPUT_H_
