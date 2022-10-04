#include <stdint.h>

#include "RefCount.h"
#include "Mesh.h"

#ifndef WIIDARTS_FONTATLAS_H_
#define WIIDARTS_FONTATLAS_H_
namespace wiidarts {
	class FontAtlas : public RefCount
	{
	public:
		virtual bool getValid() const = 0;
		virtual void setMeshCharUV(char letter, Mesh& charMesh) const = 0;
		virtual float getCharWidth(char letter) const = 0;
		virtual float getCharHeight(char letter) const = 0;
		virtual float getMaxCharHeight() const = 0;
		virtual void bindTexture(uint8_t slot) = 0;
		virtual bool hasChar(char letter) const = 0;
		virtual char getErrorChar() const = 0;
	};
}
#endif // !WIIDARTS_FONTATLAS_H_