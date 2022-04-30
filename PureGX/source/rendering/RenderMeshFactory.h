#ifndef RENDER_MESH_FACTORY_H_
#define RENDER_MESH_FACTORY_H_

#define RM3D_RENDERMESH_COUNT 2

#include "room_img.h"
#include "room_mesh.h"
#define RM3D_ROOM 0

#include "dart_img.h"
#include "dart_mesh.h"
#define RM3D_DART 1

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

	~RenderMeshFactory() {
		for (uint32_t i = 0; i < RM3D_RENDERMESH_COUNT; ++i)
			_renderMeshArray[i].Uninitialize();

		free(_renderMeshArray);
	}

private:
	RenderMesh3D* _renderMeshArray;
	RenderMeshFactory() {
		_renderMeshArray = (RenderMesh3D*)calloc(RM3D_RENDERMESH_COUNT, sizeof(RenderMesh3D));
		_renderMeshArray[RM3D_ROOM].Initialize(room_mesh, room_img, RM3D_ROOM);
		_renderMeshArray[RM3D_DART].Initialize(dart_mesh, dart_img, RM3D_DART);
	}

	RenderMeshFactory(RenderMeshFactory const&);
	void operator=(RenderMeshFactory const&);

};
#endif // !RENDER_MESH_FACTORY_H_
