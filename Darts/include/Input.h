#ifndef DARTS_INPUT_H_
#define DARTS_INPUT_H_
#include "Core.h"

namespace darts {
	class EngineCore;

	class Input
	{
	public:
		~Input();

	private:
		friend class EngineCore;

		Input();

		// Updates WPAD inputs
		void PollEvents();
	};
}
#endif // !DARTS_INPUT_H_
