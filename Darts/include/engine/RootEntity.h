#ifndef DARTS_ROOT_ENTITY_H_
#define DARTS_ROOT_ENTITY_H_
#include "Core.h"
#include "Entity.h"

class Engine;

class RootEntity : public Entity
{
public:
	RootEntity() : Entity(true) { Entity::m_identityTransform = false; }
	virtual ~RootEntity() = default;
	WD_NOCOPY(RootEntity);

protected:
	uint8_t m_userData[64];

private:
	friend class Engine;
	using Entity::ResetTransform;
	using Entity::UpdateTransform;
	using Entity::Enable;
	using Entity::Disable;
	using Entity::Update;
	using Entity::Render;
	using Entity::Load;
	using Entity::Unload;
};
#endif // !DARTS_ROOT_ENTITY_H_
