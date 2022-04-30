#ifndef ENTITY_TEST_ENTITY_H_
#define ENTITY_TEST_ENTITY_H_
#include <cstring>

#include "Entity.h"
#include "../rendering/RenderMeshFactory.h"
#include "../rendering/RenderMesh3D.h"
#include "../rendering/Renderer.h"

class TestEntity : public Entity
{
public:
	TestEntity() : Entity(Vector3f(-5, 0, -4), Vector3f(0, 90, 0), Vector3f(1)), _renderMesh(RenderMeshFactory::getInstance().getRenderMesh(RM3D_DART)) {	}

	void onUpdate(Mtx& matrix) override {
		//Vector3f pos(0, 0, 0);
		//Renderer::getInstance().drawString("test", pos, 1);
		rotate(Vector3f(1, 0, 0));
		Renderer::getInstance().drawRenderMesh(_renderMesh, matrix);
	}

private:
	RenderMesh3D* _renderMesh;
};

#endif // !ENTITY_TEST_ENTITY_H_
