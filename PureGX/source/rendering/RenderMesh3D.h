#ifndef RENDER_MESH_3D_H_
#define RENDER_MESH_3D_H_
#include <stdint.h>
#include <cstring>

#include "Mesh.h"
#include "Texture.h"

class RenderMesh3D
{
public:
	RenderMesh3D(const void* meshData, const void* textureData, uint32_t id) {
		Initialize(meshData, textureData, id);
	}

	RenderMesh3D() {
		Uninitialize();
	}

	~RenderMesh3D() {
		Uninitialize();
	}

	void Initialize(const void* meshData, const void* textureData, uint32_t id) {
		_id = id;
		_mesh.Initialize(meshData);
		_texture.Initialize(textureData);
	}

	void Uninitialize() {
		_id = 0;
		_mesh.Uninitialize();
		_texture.Uninitialize();
	}

	Texture* getTexture() { return &_texture; }
	Mesh* getMesh() { return &_mesh; }
	const uint32_t getId() const { return _id; }

private:
	uint32_t _id;
	Mesh _mesh;
	Texture _texture;

	void operator=(RenderMesh3D const& other);
};
#endif // !RENDER_MESH_3D_H_
