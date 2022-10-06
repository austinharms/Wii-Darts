#include "RenderEntity.h"

#include "core/Renderer.h"

namespace wiidarts {
	RenderEntity::RenderEntity(Mesh* mesh, Texture* texture)
	{
		_mesh = nullptr;
		_texture = nullptr;
		setMesh(mesh);
		setTexture(texture);
	}

	RenderEntity::~RenderEntity() {
		setMesh(nullptr);
		setTexture(nullptr);
	}

	Transform& RenderEntity::getTransform() { return _transform; }

	void RenderEntity::setMesh(Mesh* mesh)
	{
		if (_mesh) _mesh->drop();
		_mesh = mesh;
		if (_mesh) _mesh->grab();
	}

	void RenderEntity::setTexture(Texture* texture)
	{
		if (_texture) _texture->drop();
		_texture = texture;
		if (_texture) _texture->grab();
	}

	Mesh* RenderEntity::getMesh() const
	{
		return _mesh;
	}

	Texture* RenderEntity::getTexture() const
	{
		return _texture;
	}

	void RenderEntity::onUpdate()
	{
		if (_texture && _texture->getValid()) _texture->bind();
		if (_mesh && _mesh->getValid()) Renderer::getInstance().drawMesh3D(*_mesh);
	}
}