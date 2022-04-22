#ifndef MESH_H_
#define MESH_H_
#include <stdint.h>

class Mesh
{
public:
	const static uint32_t stride = sizeof(float) * 5;

	Mesh(const void* meshData) : _vertexCount(((const uint32_t*)meshData)[0]) {
		_buffer = ((const float*)meshData) + 1;
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
