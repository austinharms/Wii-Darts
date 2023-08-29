#include "CameraEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

CameraEntity::CameraEntity() {
	m_fov = 60;
	m_near = 0.01f;
	m_far = 100;
}

CameraEntity::~CameraEntity() {}

void CameraEntity::SetFOV(float fov)
{
	m_fov = fov;
}

WD_NODISCARD float CameraEntity::GetFOV() const
{
	return m_fov;
}

void CameraEntity::SetNearPlane(float near)
{
	m_near = near;
}

WD_NODISCARD float CameraEntity::GetNearPlane() const
{
	return m_near;
}

void CameraEntity::SetFarPlane(float far)
{
	m_far = far;
}

WD_NODISCARD float CameraEntity::GetFarPlane() const
{
	return m_far;
}

void CameraEntity::OnUpdate() {
	Transform t;
	GetWorldTransform(t);
	guVector up = t.GetUp();
	guVector forward = t.GetForward();
	guVector pos = t.GetPosition();
	guVecAdd(&pos, &forward, &forward);
	t.Reset();
	guLookAt(t.GetMatrix(), &pos, &up, &forward);
	Renderer& renderer = Engine::GetRenderer();
	renderer.SetCameraTransform(t);
	renderer.SetFOV(m_fov);
	renderer.SetClippingPlanes(m_near, m_far);
}