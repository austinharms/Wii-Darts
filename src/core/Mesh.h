#include "RefCount.h"

#ifndef WIIDARTS_MESH_H_
#include <stdint.h>
#include <gctypes.h>

#define WIIDARTS_MESH_H_
namespace wiidarts {
	class Mesh : public RefCount
	{
	public:
		static const uint16_t FLOATS_PER_VERTEX = 5;
		Mesh(uint8_t* meshBuffer);
		Mesh(uint16_t vertexCount, uint16_t indexCount, const uint16_t* indexBuffer = nullptr, const float* vertexBuffer = nullptr, bool locked = false);
		Mesh();
		Mesh(const Mesh& other);
		virtual ~Mesh();
		bool getValid() const;
		bool getLocked() const;
		void lock();
		uint16_t getIndexCount() const;
		uint16_t getVertexCount() const;
		uint32_t getFloatCount() const;
		const float* getVertexBufferPtr() const;
		const uint16_t* getIndexBufferPtr() const;

	private:
		float* _vertexBuffer;
		uint16_t* _indexBuffer;
		uint16_t _indexCount;
		uint16_t _vertexCount;
		bool _locked;
		bool _allocatedBuffer;

		Mesh& operator=(const Mesh& other);
	};
}
#endif // !WIIDARTS_H_
