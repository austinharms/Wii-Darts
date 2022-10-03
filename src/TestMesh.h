#include "core/Mesh.h"

#ifndef WIIDARTS_TESTMESH_H_
namespace wiidarts {
	class TestMesh : public Mesh
	{
	public:
		TestMesh() : Mesh(VERTEX_COUNT, INDEX_COUNT, INDEX_BUFFER, VERTEX_BUFFER) {}
		virtual ~TestMesh() {}

	private:
		const static uint16_t VERTEX_COUNT;
		const static uint16_t INDEX_COUNT;
		const static float VERTEX_BUFFER[];
		const static uint16_t INDEX_BUFFER[];
	};

	const uint16_t TestMesh::VERTEX_COUNT = 4;
	const uint16_t TestMesh::INDEX_COUNT = 6;
	const float TestMesh::VERTEX_BUFFER[VERTEX_COUNT * Mesh::FLOATS_PER_VERTEX] = {
		0, 0, 0, 0, 0,
		1, 0, 0, 1, 0,
		1, 1, 0, 1, 1,
		0, 1, 0, 0, 1
	};
	const uint16_t TestMesh::INDEX_BUFFER[INDEX_COUNT] = {
		0, 1, 2,
		0, 2, 3
	};
}
#endif // !WIIDARTS_TESTMESH_H_