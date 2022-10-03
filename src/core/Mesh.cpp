#include "Mesh.h"

#include <cstring>
#include <malloc.h>

#include "Logger.h"

namespace wiidarts {
	Mesh::Mesh(const uint8_t* meshBuffer)
	{
		_locked = true;
		_allocatedBuffer = false;
		_vertexCount = (meshBuffer[0] << 8) + meshBuffer[1];
		_indexCount = (meshBuffer[2] << 8) + meshBuffer[3];
		_vertexBuffer = (float*)(meshBuffer + 4);
		_indexBuffer = (uint16_t*)(_vertexBuffer + getFloatCount());
	}

	Mesh::Mesh(uint16_t vertexCount, uint16_t indexCount, const uint16_t* indexBuffer, const float* vertexBuffer, bool locked)
	{
		_locked = false;
		_allocatedBuffer = true;
		_vertexCount = vertexCount;
		_indexCount = indexCount;
		_vertexBuffer = (float*)calloc(getFloatCount(), sizeof(float));
		_indexBuffer = (uint16_t*)calloc(_indexCount, sizeof(uint16_t));
		if (_indexBuffer == nullptr || _vertexBuffer == nullptr) {
			Logger::error("Mesh::Mesh failed to allocate Mesh buffers");
			_indexCount = 0;
			_vertexCount = 0;
			if (_vertexBuffer)
				free(_vertexBuffer);
			if (_indexBuffer)
				free(_indexBuffer);
		}

		if (vertexBuffer)
			memcpy(_vertexBuffer, vertexBuffer, sizeof(float) * getFloatCount());
		if (indexBuffer)
			memcpy(_indexBuffer, indexBuffer, sizeof(uint16_t) * _indexCount);
	}

	Mesh::Mesh()
	{
		_locked = false;
		_allocatedBuffer = true;
		_indexCount = 0;
		_vertexCount = 0;
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
	}

	Mesh::Mesh(const Mesh& other)
	{
		_locked = false;
		_allocatedBuffer = true;
		if (other.getValid()) {
			_indexCount = other._indexCount;
			_vertexCount = other._vertexCount;
			_vertexBuffer = (float*)malloc(sizeof(float) * getFloatCount());
			_indexBuffer = (uint16_t*)malloc(sizeof(uint16_t) * _indexCount);
			if (_indexBuffer == nullptr || _vertexBuffer == nullptr) {
				Logger::error("Mesh::Mesh failed to allocate Mesh buffers");
				_indexCount = 0;
				_vertexCount = 0;
				if (_vertexBuffer)
					free(_vertexBuffer);
				if (_indexBuffer)
					free(_indexBuffer);
			}

			memcpy(_vertexBuffer, other._vertexBuffer, sizeof(float) * getFloatCount());
			memcpy(_indexBuffer, other._indexBuffer, sizeof(uint16_t) * _indexCount);
		}
		else {
			Logger::warn("Mesh::Mesh attempted to copy invalid Mesh");
			_indexCount = 0;
			_vertexCount = 0;
			_vertexBuffer = nullptr;
			_indexBuffer = nullptr;
		}
	}

	Mesh::~Mesh()
	{
		if (_allocatedBuffer) {
			if (_vertexBuffer)
				free(_vertexBuffer);
			if (_indexBuffer)
				free(_indexBuffer);
		}

		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;
		_indexCount = 0;
		_vertexCount = 0;
	}

	bool Mesh::getValid() const
	{
		return _indexCount != 0 && _vertexCount != 0 && _vertexBuffer != nullptr && _indexBuffer != nullptr;
	}

	bool Mesh::getLocked() const
	{
		return _locked;
	}

	void Mesh::lock()
	{
		_locked = true;
	}

	uint16_t Mesh::getIndexCount() const
	{
		return _indexCount;
	}

	uint16_t Mesh::getVertexCount() const
	{
		return _vertexCount;
	}

	uint32_t Mesh::getFloatCount() const
	{
		return (uint32_t)_vertexCount * FLOATS_PER_VERTEX;
	}

	const float* Mesh::getVertexBufferPtr() const
	{
		return _vertexBuffer;
	}

	const uint16_t* Mesh::getIndexBufferPtr() const
	{
		return _indexBuffer;
	}
}