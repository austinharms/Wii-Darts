#ifndef DARTS_CAMERA_ENTITY_H_
#define DARTS_CAMERA_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"

class CameraEntity : public Entity
{
public:
	CameraEntity();
	virtual ~CameraEntity();
	void SetFOV(float fov);
	WD_NODISCARD float GetFOV() const;
	void SetNearPlane(float near);
	WD_NODISCARD float GetNearPlane() const;
	void SetFarPlane(float far);
	WD_NODISCARD float GetFarPlane() const;
	using Entity::GetTransform;
	using Entity::SetTransform;
	using Entity::Enable;
	using Entity::Disable;

protected:
	void OnUpdate() WD_OVERRIDE;

private:
	float m_fov;
	float m_near;
	float m_far;
};
#endif // !DARTS_CAMERA_ENTITY_H_
