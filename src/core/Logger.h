#include <stdint.h>
#include <cstdlib>

#include "Font.h"

#ifndef WIIDARTS_LOGGER_H_
#define WIIDARTS_LOGGER_H_
namespace wiidarts {
	namespace Logger {
		void setFont(Font* font);
		void debug(const char* msg);
		void info(const char* msg);
		void warn(const char* msg);
		void error(const char* msg);
		void fatal(const char* msg, int code = EXIT_FAILURE);
		void print(const char* prefix, const char* msg, uint32_t color);
		extern Font* LogFont;
	}
}
#endif // !WIIDARTS_LOGGER_H_
