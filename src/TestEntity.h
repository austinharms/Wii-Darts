#include <new>

#include "core/Entity.h"
#include "core/Renderer.h"
#include "core/Texture.h"
#include "core/Mesh.h"
#include "Prefabs.h"

#ifndef WIIDARTS_TESTENTITY_H_
#define WIIDARTS_TESTENTITY_H_
namespace wiidarts {
	class TestEntity : public Entity
	{
	public:
		TestEntity() {
			_mesh = nullptr;
			_texture = nullptr;

			_texture = Prefabs::DartTexture;
			if (_texture == nullptr || !_texture->getValid()) {
				_texture = nullptr;
				return;
			}

			_texture->grab();
			_mesh = Prefabs::DartMesh;
			if (_mesh == nullptr || !_mesh->getValid()) {
				_texture->drop();
				_texture = nullptr;
				_mesh = nullptr;
				return;
			}

			_mesh->grab();
			_transform.rotate({ 0, -M_PI/2, 0 });
			_transform.setPosition({ 0, 0, 0 });
			//_transform.scale({ 1, 1, 1 });
		}

		virtual ~TestEntity() {
			if (_texture) _texture->drop();
			if (_mesh) _mesh->drop();
			_texture = nullptr;
			_mesh = nullptr;
		}

	protected:
		void onUpdate() override {
			if (_texture == nullptr || _mesh == nullptr) return;
			_texture->bind();
			Renderer::getInstance().drawMesh3D(*_mesh);
			_transform.rotate({ 0.05f, 0, 0 });
		}

	private:
		Texture* _texture;
		Mesh* _mesh;
	};
}
#endif // !WIIDARTS_TESTENTITY_H_
