#include <stdint.h>

#include "Entity.h"
#include "Mesh.h"
#include "Texture.h"

#ifndef WIIDARTS_RENDER_ENTITY_H_
#define WIIDARTS_RENDER_ENTITY_H_
namespace wiidarts {
	class RenderEntity : public Entity
	{
	public:
		RenderEntity(Mesh* mesh, Texture* texture, uint32_t color = 0xffffffff);
		virtual ~RenderEntity();
		void setMesh(Mesh* mesh);
		void setTexture(Texture* texture);
		Mesh* getMesh() const;
		Texture* getTexture() const;
		void setColor(uint32_t color);
		uint32_t getColor() const;
		void onUpdate() override;

	private:
		Mesh* _mesh;
		Texture* _texture;
		uint32_t _color;
	};
}
#endif // !WIIDARTS_RENDER_ENTITY_H_
