#ifndef ENTITY_TEST_ENTITY_H_
#define ENTITY_TEST_ENTITY_H_
#include "Entity.h"
#include "../rendering/RenderMeshFactory.h"
#include "../rendering/RenderMesh3D.h"
#include "../rendering/Renderer.h"

class TestEntity : Entity
{
public:
	TestEntity() : _renderMesh(RenderMeshFactory::getInstance().getRenderMesh(RM3D_TEST)) {}

	void onUpdate() override {
		Renderer::getInstance().drawRenderMesh(_renderMesh, Pos);
	}

private:
	const RenderMesh3D* _renderMesh;
};

#endif // !ENTITY_TEST_ENTITY_H_
