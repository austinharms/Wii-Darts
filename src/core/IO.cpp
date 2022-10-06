#include "IO.h"

#include <wiiuse/wpad.h>

#include "Renderer.h"

namespace wiidarts {
	IO::IO() {
		// Setup the input system
		WPAD_Init();
		setWiimoteInputType(BUTTONS);
		GXRModeObj* mode = Renderer::getInstance().getMode();
		WPAD_SetVRes(WPAD_CHAN_ALL, mode->fbWidth, mode->efbHeight);
	}

	IO::~IO()
	{
	}

	int32_t IO::setWiimoteInputType(InputOptions type)
	{
		return WPAD_SetDataFormat(WPAD_CHAN_ALL, type);
	}
}