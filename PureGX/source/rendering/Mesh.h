#ifndef MESH_H_
#define MESH_H_
#include <stdint.h>

class Mesh
{
public:
	const static uint32_t stride = sizeof(float) * 5;

	Mesh(const void* meshData) : _vertexCount(((const uint32_t*)meshData)[0]) {
		_buffer = ((const float*)meshData) + 1;
		//float* buf = (float*)malloc(sizeof(float) * 6 * 5);
		//buf[0] = 0;
		//buf[1] = 0;
		//buf[2] = 0;
		//buf[3] = 0;
		//buf[4] = 0;

		//buf[5] = 1;
		//buf[6] = 1;
		//buf[7] = 0;
		//buf[8] = 1;
		//buf[9] = 1;

		//buf[10] = 0;
		//buf[11] = 1;
		//buf[12] = 0;
		//buf[13] = 0;
		//buf[14] = 1;

		//buf[15] = 0;
		//buf[16] = 0;
		//buf[17] = 0;
		//buf[18] = 0;
		//buf[19] = 0;

		//buf[20] = 1;
		//buf[21] = 0;
		//buf[22] = 0;
		//buf[23] = 1;
		//buf[24] = 0;

		//buf[25] = 1;
		//buf[26] = 1;
		//buf[27] = 0;
		//buf[28] = 1;
		//buf[29] = 1;

		//_buffer = (const float*)buf;
	}

	const uint32_t getTriCount() const {
		return _vertexCount / 3;
	}

	const uint32_t getVertCount() const {
		return _vertexCount;
	}

	const float* getVertexBuffer() const {
		return _buffer;
	}

private:
	const uint32_t _vertexCount;
	const float* _buffer;
};
#endif // !RENDER_MESH_H_
