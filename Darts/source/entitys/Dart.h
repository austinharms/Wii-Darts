#ifndef DART_ENTITY_H_
#define DART_ENTITY_H_

#include "Entity.h"
#include "../Vector3f.h"
#include "RenderEntity.h"
#include "../rendering/RenderMeshFactory.h"

class Dart : public Entity
{
public:
	Dart() : Entity(Vector3f(0), Vector3f(0), Vector3f(1)) {
		_dartModel = new RenderEntity(RM3D_DART);
		_dartModel->scale(Vector3f(0.02225f));
		_dartModel->rotate(Vector3f(0, 0, 0));
		addChild((Entity*)_dartModel);
	}

	virtual ~Dart() {
		_dartModel->drop();
	}

	void onUpdate(Mtx& matrix) override {
		_dartModel->rotate(Vector3f(0,0,4));
	}

	void setRotation(Vector3f rot) {
		rotate(rot - _rotation);
		_rotation = rot;
	}

private:
	RenderEntity* _dartModel;
	Vector3f _rotation;
};

#endif // !DART_ENTITY_H_
