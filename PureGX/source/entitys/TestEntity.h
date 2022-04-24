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
	TestEntity() : _renderMesh(RenderMeshFactory::getInstance().getRenderMesh(RM3D_TEST)) {
		_rot = 0;
		localPos.x = -4;
		localPos.z = -5;
	}

	void onUpdate() override {
		//Renderer::getInstance().drawText(100, 100, "hi");
		_rot += 1;
		if (_rot >= 360) {
			_rot -= 360;
		}

		Renderer::getInstance().drawRenderMesh(_renderMesh, getWorldPosition(), Vector3f(_rot,90,0));
	}

private:
	RenderMesh3D* _renderMesh;
	float _rot;
};

#endif // !ENTITY_TEST_ENTITY_H_
