#ifndef RENDER_MESH_3D_H_
#define RENDER_MESH_3D_H_
#include <stdint.h>

#include "Mesh.h"
#include "Texture.h"
#include <memory.h>

class RenderMesh3D
{
public:
	RenderMesh3D(const void* meshData, const uint32_t meshDataLength, const void* textureData, uint32_t i) : _id(i), _mesh(meshData, meshDataLength), _texture(textureData) {}
	const Texture* getTexture() const { return &_texture; }
	const Mesh* getMesh() const { return &_mesh; }
	const uint32_t getId() const { return _id; }
	void operator=(RenderMesh3D const& other) {
		memcpy((void*)this, (void*)&other, sizeof(RenderMesh3D));
	}

private:
	RenderMesh3D();
	Texture _texture;
	Mesh _mesh;
	const uint32_t _id;
};
#endif // !RENDER_MESH_3D_H_
