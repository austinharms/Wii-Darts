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

		const char* getCWD() const;

		// read file bytes and set size to file size if it is not null
		// returned pointer needs to be freed using free if it is not null
		// returns null on error or if the file is empty
		uint8_t* getFileBytes(const char* path, uint32_t* size = nullptr);

	private:
		IO();
		IO(const IO&);
		void operator=(const IO&);

		char* _cwd;
	};
}
#endif