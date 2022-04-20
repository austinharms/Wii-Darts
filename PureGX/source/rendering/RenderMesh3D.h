#ifndef RENDER_MESH_3D_H_
#define RENDER_MESH_3D_H_
#include <stdint.h>
#include <cstring>

#include "Mesh.h"
#include "Texture.h"

class RenderMesh3D
{
public:
	RenderMesh3D(const void* meshData, const uint32_t meshDataLength, const void* textureData, uint32_t i) : _id(i), _mesh(meshData, meshDataLength), _texture(textureData) {}
	Texture* getTexture() { return &_texture; }
	Mesh* getMesh() { return &_mesh; }
	const uint32_t getId() const { return _id; }
	void operator=(RenderMesh3D const& other) {
		memcpy((void*)this, (void*)&other, sizeof(RenderMesh3D));
	}

private:
	const uint32_t _id;
	Mesh _mesh;
	Texture _texture;

	RenderMesh3D();
};
#endif // !RENDER_MESH_3D_H_
