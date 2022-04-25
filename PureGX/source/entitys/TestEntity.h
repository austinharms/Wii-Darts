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
	TestEntity() : _renderMesh(RenderMeshFactory::getInstance().getRenderMesh(RM3D_DART)) {
		_rot = 0;
		localPos.x = -4;
		localPos.z = -5;
		localRot.x = 90;
		localRot.z = 90;
	}

	void onUpdate() override {
		//Renderer::getInstance().drawText(100, 100, "hi");
		localRot.y += 1;
		if (localRot.y >= 360) {
			localRot.y -= 360;
		}

		Renderer::getInstance().drawRenderMesh(_renderMesh, getWorldPosition(), getWorldRotation(), getWorldScale());
	}

private:
	RenderMesh3D* _renderMesh;
	float _rot;
};

#endif // !ENTITY_TEST_ENTITY_H_
