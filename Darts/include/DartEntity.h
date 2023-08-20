#ifndef DARTS_DART_H_
#define DARTS_DART_H_
#include "engine/Core.h"
#include "engine/Entity.h"
//#include "engine/TextureHandle.h"
#include "engine/RenderMeshHandle.h"

class DartEntity : public Entity
{
public:
	DartEntity();
	virtual ~DartEntity();
	void OnLoad() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;

private:
	RenderMeshHandle* m_dartCoreMesh;
	RenderMeshHandle* m_dartFinsMesh;
};
#endif // !DARTS_DART_H_
