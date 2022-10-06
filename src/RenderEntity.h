#include "core/Entity.h"
#include "core/Mesh.h"
#include "core/Texture.h"

#ifndef WIIDARTS_RENDER_ENTITY_H_
#define WIIDARTS_RENDER_ENTITY_H_
namespace wiidarts {
	class RenderEntity : public Entity
	{
	public:
		RenderEntity(Mesh* mesh, Texture* texture);
		virtual ~RenderEntity();
		Transform& getTransform();
		void setMesh(Mesh* mesh);
		void setTexture(Texture* texture);
		Mesh* getMesh() const;
		Texture* getTexture() const;
		void onUpdate() override;

	private:
		Mesh* _mesh;
		Texture* _texture;
	};
}
#endif // !WIIDARTS_RENDER_ENTITY_H_
