#ifndef DARTS_DART_ENTITY_H_
#define DARTS_DART_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"
//#include "engine/TextureHandle.h"
#include "engine/RenderMeshHandle.h"

class DartEntity : public Entity
{
public:
	DartEntity();
	virtual ~DartEntity();
	void Throw(const guVector& velocity);
	using Entity::GetTransform;

protected:
	void OnLoad() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;
	void OnUpdate() WD_OVERRIDE;

private:
	RenderMeshHandle* m_dartCoreMesh;
	RenderMeshHandle* m_dartFinsMesh;
	guVector m_velocity;
	float m_rot;
	bool m_sim;
};
#endif // !DARTS_DART_ENTITY_H_
