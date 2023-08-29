#ifndef DARTS_CAMERA_ENTITY_H_
#define DARTS_CAMERA_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"

#ifndef WD_DEBUG_CAMERA_MTX
#define WD_DEBUG_CAMERA_MTX 0
#endif // !WD_DEBUG_CAMERA_MTX

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
#if WD_DEBUG_CAMERA_MTX
	void OnRender() WD_OVERRIDE;
#endif

private:
	float m_fov;
	float m_near;
	float m_far;
};
#endif // !DARTS_CAMERA_ENTITY_H_
