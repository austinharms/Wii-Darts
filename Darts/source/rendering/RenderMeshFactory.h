#ifndef RENDER_MESH_FACTORY_H_
#define RENDER_MESH_FACTORY_H_

#define RM3D_RENDERMESH_COUNT 1

#include "test_png.h"
#define RM3D_TEST 0

#include <stdint.h>
#include "RenderMesh3D.h"

class RenderMeshFactory
{
public:
	static RenderMeshFactory& getInstance()
	{
		static RenderMeshFactory instance;
		return instance;
	}

	const RenderMesh3D* getRenderMesh(uint32_t id) const {
		if (id >= RM3D_RENDERMESH_COUNT) return nullptr;
		return &_renderMeshArray[id];
	}

private:
	RenderMesh3D _renderMeshArray[RM3D_RENDERMESH_COUNT];
	RenderMeshFactory() {
		_renderMeshArray[RM3D_TEST] = RenderMesh3D(nullptr, 0, test_png, RM3D_TEST);
	}

	RenderMeshFactory(RenderMeshFactory const&);
	void operator=(RenderMeshFactory const&);

};
#endif // !RENDER_MESH_FACTORY_H_
