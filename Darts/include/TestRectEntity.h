#ifndef DARTS_TEST_RECT_ENTITY_H_
#define DARTS_TEST_RECT_ENTITY_H_
#include "Core.h"
#include "Entity.h"
#include "RenderMesh.h"

namespace darts {
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
}
#endif // !DARTS_TEST_RECT_ENTITY_H_
