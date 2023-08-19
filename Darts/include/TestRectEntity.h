#ifndef DARTS_TEST_RECT_ENTITY_H_
#define DARTS_TEST_RECT_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"
#include "engine/RenderMeshHandle.h"
#include "engine/TextureHandle.h"

class TestRectEntity : public Entity
{
public:
	TestRectEntity();
	virtual ~TestRectEntity();

protected:
	void OnLoad() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;

private:
	RenderMeshHandle* m_mesh;
	TextureHandle* m_texture;
};
#endif // !DARTS_TEST_RECT_ENTITY_H_
