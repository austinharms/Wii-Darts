#include "CameraEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

CameraEntity::CameraEntity() {

}

CameraEntity::~CameraEntity() {}

void CameraEntity::OnUpdate() {
	Transform t;
	GetWorldTransform(t);
	guVector up = t.GetUp();
	guVector forward = t.GetForward();
	guVector pos = t.GetPosition();
	guVecAdd(&pos, &forward, &forward);
	t.Reset();
	guLookAt(t.GetMatrix(), &pos, &up, &forward);
	Engine::GetRenderer().SetCameraTransform(t);
}