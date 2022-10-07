#include "IO.h"

#include <wiiuse/wpad.h>
#include <fat.h>

#include "Renderer.h"
#include "Logger.h"

namespace wiidarts {
	IO::IO() {
		// Setup the input system
		WPAD_Init();
		setWiimoteInputType(BUTTONS);
		GXRModeObj* mode = Renderer::getInstance().getMode();
		WPAD_SetVRes(WPAD_CHAN_ALL, mode->fbWidth, mode->efbHeight);
		if (!fatInitDefault()) Logger::fatal("Failed to init Fat file system");
	}

	IO::~IO()
	{
	}

	int32_t IO::setWiimoteInputType(InputOptions type)
	{
		return WPAD_SetDataFormat(WPAD_CHAN_ALL, type);
	}

	void IO::updateInputs()
	{
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if (pressed & WPAD_BUTTON_HOME) exit(0);
	}
}