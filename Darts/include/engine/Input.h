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
	bool GetIRScreenPose(float* x, float* y);
	bool GetButtonDown(uint32_t button);
	bool GetButtonReleased(uint32_t button);
	bool GetButtonPressed(uint32_t button);
	bool GetOrientation(guVector& out);
	bool GetAccel(guVector& out);
	bool GetIRScreenPose(uint8_t ctrlNo, float* x, float* y);
	bool GetButtonDown(uint8_t ctrlNo, uint32_t button);
	bool GetButtonReleased(uint8_t ctrlNo, uint32_t button);
	bool GetButtonPressed(uint8_t ctrlNo, uint32_t button);
	bool GetOrientation(uint8_t ctrlNo, guVector& out);
	bool GetAccel(uint8_t ctrlNo, guVector& out);
	void SetActiveController(uint8_t ctrlNo);
	WD_NODISCARD uint8_t GetActiveController() const;

private:
	friend class Engine;

	uint8_t m_activeController;
	bool m_inputDisabled;
	bool m_init;

	Input();

	void Init();
	void EnableInputs();
	void DisableInputs();
	// Updates WPAD inputs
	void PollEvents();
};
#endif // !DARTS_INPUT_H_
