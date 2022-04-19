#ifndef ENTITY_TEST_ENTITY_H_
#define ENTITY_TEST_ENTITY_H_
#include "Entity.h"
#include "../rendering/RenderMeshFactory.h"
#include "../rendering/RenderMesh3D.h"
#include "../rendering/Renderer.h"

class TestEntity : public Entity
{
public:
	TestEntity() : _renderMesh(RenderMeshFactory::getInstance().getRenderMesh(RM3D_TEST)) {}

	void onUpdate() override {
		//Renderer::getInstance().drawText(100, 100, "hi");
		Renderer::getInstance().drawRenderMesh(_renderMesh, getWorldPosition());
	}

private:
	RenderMesh3D* _renderMesh;
};

#endif // !ENTITY_TEST_ENTITY_H_
