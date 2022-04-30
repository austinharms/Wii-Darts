#ifndef ENTITY_RENDER_ENTITY_H_
#define ENTITY_RENDER_ENTITY_H_
#include <cstring>

#include "Entity.h"
#include "../rendering/RenderMeshFactory.h"
#include "../rendering/RenderMesh3D.h"
#include "../rendering/Renderer.h"

class RenderEntity : public Entity
{
public:
	RenderEntity(uint32_t renderMeshId) : Entity(Vector3f(0), Vector3f(0), Vector3f(1)), _renderMesh(RenderMeshFactory::getInstance().getRenderMesh(renderMeshId)) {	}

	void onUpdate(Mtx& matrix) override {
		Renderer::getInstance().drawRenderMesh(_renderMesh, matrix);
	}

private:
	RenderMesh3D* _renderMesh;
};

#endif // !ENTITY_TEST_ENTITY_H_
