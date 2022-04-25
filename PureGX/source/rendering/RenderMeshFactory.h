#ifndef RENDER_MESH_FACTORY_H_
#define RENDER_MESH_FACTORY_H_

#define RM3D_RENDERMESH_COUNT 1

#include "dart_img.h"
#include "dart_mesh.h"
#define RM3D_DART 0

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
		_renderMeshArray[RM3D_DART] = RenderMesh3D(dart_mesh, dart_img, RM3D_DART);
	}

	RenderMeshFactory(RenderMeshFactory const&);
	void operator=(RenderMeshFactory const&);

};
#endif // !RENDER_MESH_FACTORY_H_
