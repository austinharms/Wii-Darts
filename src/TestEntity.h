#include <new>

#include "core/Entity.h"
#include "core/Renderer.h"
#include "core/Texture.h"
#include "core/Mesh.h"
#include "board_RGBA8.h"
#include "board_mesh.h"

#ifndef WIIDARTS_TESTENTITY_H_
#define WIIDARTS_TESTENTITY_H_
namespace wiidarts {
	class TestEntity : public Entity
	{
	public:
		TestEntity() {
			_mesh = nullptr;
			_texture = nullptr;

			_texture = new(std::nothrow) Texture(board_RGBA8, false, true, true);
			if (_texture == nullptr || !_texture->getValid()) {
				if (_texture) _texture->drop();
				_texture = nullptr;
				return;
			}

			_mesh = new(std::nothrow) Mesh(board_mesh);
			if (_mesh == nullptr || !_mesh->getValid()) {
				_texture->drop();
				_texture = nullptr;
				if (_mesh)
					_mesh->drop();
				_mesh = nullptr;
				return;
			}

			_transform.rotate({ 90, 0, 0 });
			_transform.setPosition({ 0, 0, -6 });
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
			//_transform.rotate({ 0,0.05f,0 });
		}

	private:
		Texture* _texture;
		Mesh* _mesh;
	};
}
#endif // !WIIDARTS_TESTENTITY_H_
