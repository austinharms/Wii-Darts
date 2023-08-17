#ifndef DARTS_TEST_RECT_ENTITY_H_
#define DARTS_TEST_RECT_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"
#include "engine/RenderMesh.h"

class TestRectEntity : public Entity
{
public:
	TestRectEntity();
	virtual ~TestRectEntity();

protected:
	void OnRender() WD_OVERRIDE;

private:
	RenderMesh m_mesh;
};
#endif // !DARTS_TEST_RECT_ENTITY_H_
