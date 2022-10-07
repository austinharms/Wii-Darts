#include <stdint.h>

#ifndef WIIDARTS_IO_H_
#define WIIDARTS_IO_H_
namespace wiidarts {
	class IO
	{
	public:
		enum InputOptions
		{
			BUTTONS = 0,
			BUTTONS_ACCELEROMETER,
			BUTTONS_ACCELEROMETER_POINTER
		};

		static IO& getInstance()
		{
			static IO instance;
			return instance;
		}

		~IO();

		// returns 0 on success 
		int32_t setWiimoteInputType(InputOptions type);

		void updateInputs();

	private:
		IO();
		IO(const IO&);
		void operator=(const IO&);
	};
}
#endif