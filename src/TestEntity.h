#include <new>

#include "core/Entity.h"
#include "core/Renderer.h"
#include "core/Texture.h"
#include "TestMesh.h"

#ifndef WIIDARTS_TESTENTITY_H_
#define WIIDARTS_TESTENTITY_H_
namespace wiidarts {
	class TestEntity : public Entity
	{
	public:
		TestEntity() {
			_mesh = nullptr;
			_texture = nullptr;
			_texture = new(std::nothrow) Texture(1, 1);
			if (_texture == nullptr || !_texture->getValid()) {
				if (_texture) _texture->drop();
				_texture = nullptr;
				return;
			}

			_mesh = new(std::nothrow) TestMesh();
			if (_mesh == nullptr || !_mesh->getValid()) {
				_texture->drop();
				_texture = nullptr;
				if (_mesh)
					_mesh->drop();
				_mesh = nullptr;
				return;
			}

			_texture->setPixelRGBA(0, 0, 0x0000ffff);
			_transform.setPosition({ 0.5f, 0, 0 });
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
		}

	private:
		Texture* _texture;
		Mesh* _mesh;
	};
}
#endif // !WIIDARTS_TESTENTITY_H_
