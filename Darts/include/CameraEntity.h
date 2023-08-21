#ifndef DARTS_CAMERA_ENTITY_H_
#define DARTS_CAMERA_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"

class CameraEntity : public Entity
{
public:
	CameraEntity();
	virtual ~CameraEntity();

protected:
	void OnUpdate() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;

private:
	float m_set[6];
};
#endif // !DARTS_CAMERA_ENTITY_H_
