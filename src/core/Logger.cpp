#include "Logger.h"

#include <new>

#include "../Prefabs.h"
#include "Renderer.h"
#include "FontAtlas.h"
#include "BasicFontAtlas.h"

namespace wiidarts {
	namespace Logger {
		Font* LogFont = new(std::nothrow) Font(Prefabs::DefaultFontAtlas);

		void setFont(Font* font)
		{
			if (LogFont) LogFont->drop();
			LogFont = font;
			if (font) font->grab(); 
		}

		void debug(const char* msg) {
			print("[DEBUG]", msg, 0xffffffff);
		}

		void info(const char* msg) {
			print("[INFO]", msg, 0xffffffff);
		}

		void warn(const char* msg) {
			print("[WARN]", msg, 0xffff00ff);
		}

		void error(const char* msg) {
			print("[ERROR]", msg, 0xff0000ff);
		}

		void fatal(const char* msg, int code)
		{
			print("[FATAL]", msg, 0xff0000ff);
			exit(-1);
		}

		void print(const char* prefix, const char* msg, uint32_t color)
		{
			if (LogFont && LogFont->getValid()) {
				LogFont->setColor(color);
				LogFont->drawText(prefix);
				LogFont->drawText(msg);
			}
		}
	}
}