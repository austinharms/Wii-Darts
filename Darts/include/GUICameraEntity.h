#ifndef DARTS_GUI_CAMREA_ENTITY_H_
#define DARTS_GUI_CAMREA_ENTITY_H_
#include "engine/Core.h"
#include "engine/CameraEntity.h"

class GUICameraEntity : public CameraEntity
{
public:
	GUICameraEntity();
	~GUICameraEntity();

protected:
	void OnUpdate() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;

private:
	typedef CameraEntity Super;

	float m_set[6];
};
#endif // !DARTS_GUI_CAMREA_ENTITY_H_