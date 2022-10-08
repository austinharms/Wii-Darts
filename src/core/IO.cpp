#include "IO.h"

#include <wiiuse/wpad.h>
#include <fat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <malloc.h>

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
		if (chdir("sd:/apps/darts/") != 0) Logger::fatal("Failed to set working directory");
		if ((_cwd = getcwd(nullptr, 0)) == nullptr) Logger::fatal("Failed to get working directory");
	}

	IO::~IO()
	{
		free(_cwd);
	}

	int32_t IO::setWiimoteInputType(InputOptions type)
	{
		_currentInputType = type;
		return WPAD_SetDataFormat(WPAD_CHAN_ALL, type);
	}

	void IO::rumbleController(int8_t controllerNumber, bool active)
	{
		WPAD_Rumble(controllerNumber, active);
	}

	bool IO::getControllerIRScreenPos(uint8_t controllerNumber, float* x, float* y)
	{
		if (_currentInputType != BUTTONS_ACCELEROMETER_POINTER)	return false;
		WPADData* data = WPAD_Data(controllerNumber);
		*x = data->ir.x;
		*y = Renderer::getInstance().getScreenHeight() - data->ir.y;
		return true;
	}

	bool IO::getControllerButtonDown(uint8_t controllerNumber, uint32_t button)
	{
		return WPAD_ButtonsHeld(controllerNumber) & button;
	}

	bool IO::getControllerButtonPressed(uint8_t controllerNumber, uint32_t button)
	{
		return WPAD_ButtonsDown(controllerNumber) & button;
	}

	bool IO::getControllerButtonReleased(uint8_t controllerNumber, uint32_t button)
	{
		return WPAD_ButtonsUp(controllerNumber) & button;
	}

	void IO::updateInputs()
	{
		WPAD_ScanPads();
		if (getControllerButtonDown(0, WPAD_BUTTON_HOME)) exit(0);
	}

	const char* IO::getCWD() const
	{
		return _cwd;
	}

	uint8_t* IO::getFileBytes(const char* path, uint32_t* size)
	{
		std::ifstream file(path, std::ios::binary);
		file.seekg(0, std::ios::end);
		int32_t endPos = file.tellg();
		if (endPos <= 0) return nullptr;
		file.seekg(0, std::ios::beg);
		uint8_t* buf = (uint8_t*)malloc(endPos);
		if (!buf) return nullptr;
		file.read((char*)buf, endPos);
		if (size != nullptr) *size = (uint32_t)endPos;
		return buf;
	}
}