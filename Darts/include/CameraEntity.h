#ifndef DARTS_CAMERA_ENTITY_H_
#define DARTS_CAMERA_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"

class CameraEntity : public Entity
{
public:
	CameraEntity();
	virtual ~CameraEntity();
	using Entity::GetTransform;
	using Entity::SetTransform;
	using Entity::Enable;
	using Entity::Disable;

protected:
	void OnUpdate() WD_OVERRIDE;

private:

};
#endif // !DARTS_CAMERA_ENTITY_H_
