#ifndef RENDER_MESH_FACTORY_H_
#define RENDER_MESH_FACTORY_H_

#define RM3D_RENDERMESH_COUNT 1

#include "test_png.h"
#include "test_mesh.h"
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

	RenderMesh3D* getRenderMesh(uint32_t id) const {
		if (id >= RM3D_RENDERMESH_COUNT) return nullptr;
		return &_renderMeshArray[id];
	}

private:
	RenderMesh3D* _renderMeshArray;
	RenderMeshFactory() {
		_renderMeshArray = (RenderMesh3D*)malloc(RM3D_RENDERMESH_COUNT * sizeof(RenderMesh3D));
		_renderMeshArray[RM3D_TEST] = RenderMesh3D(RM3D_TEST, test_mesh, test_mesh_size, test_png);
	}

	RenderMeshFactory(RenderMeshFactory const&);
	void operator=(RenderMeshFactory const&);

};
#endif // !RENDER_MESH_FACTORY_H_
