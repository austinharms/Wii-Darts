#ifndef WIIDARTS_LOGGER_H_
#define WIIDARTS_LOGGER_H_
namespace wiidarts {
	namespace Logger {
		inline void debug(const char* msg);
		inline void info(const char* msg);
		inline void warn(const char* msg);
		inline void error(const char* msg);
	}
}

#include "Logger.inl"
#endif // !WIIDARTS_LOGGER_H_
