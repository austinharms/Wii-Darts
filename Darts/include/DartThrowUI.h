#ifndef DARTS_DART_THROW_UI_H_
#define DARTS_DART_THROW_UI_H_
#include "engine/Core.h"
#include "engine/Entity.h"

class DartEntity;
class CameraEntity;

class DartThrowUI : public Entity
{
public:
	DartThrowUI();
	virtual ~DartThrowUI();
	using Entity::Enable;
	using Entity::Disable;
	void SetThrowPower(float strength, float swing);

protected:
	void OnRender() WD_OVERRIDE;

private:
	float m_strength;
	float m_swing;
};
#endif // !DARTS_DART_THROW_UI_H_