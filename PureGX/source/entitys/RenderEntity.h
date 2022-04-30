#ifndef ENTITY_RENDER_ENTITY_H_
#define ENTITY_RENDER_ENTITY_H_
#include <cstring>

#include "Entity.h"
#include "../rendering/RenderMeshFactory.h"
#include "../rendering/RenderMesh3D.h"
#include "../rendering/Renderer.h"

class RenderEntity : public Entity
{
public:
	RenderEntity(uint32_t renderMeshId) : Entity(Vector3f(0), Vector3f(0), Vector3f(1)), _renderMesh(RenderMeshFactory::getInstance().getRenderMesh(renderMeshId)) {	}

	void onUpdate(Mtx& matrix) override {
		Renderer::getInstance().drawRenderMesh(_renderMesh, matrix);
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 6);
		Vector3f pos(0);
		GX_Position3f32(0 + pos.x, 0 + pos.y, 0 + pos.z);
		GX_Color1u32(0xffffffff);
		GX_TexCoord2f32(0.0f, 0.0f);

		GX_Position3f32(1 + pos.x, 1 + pos.y, 0 + pos.z);
		GX_Color1u32(0xffffffff);
		GX_TexCoord2f32(1.0f, 1.0f);

		GX_Position3f32(0 + pos.x, 1 + pos.y, 0 + pos.z);
		GX_Color1u32(0xffffffff);
		GX_TexCoord2f32(0.0f, 1.0f);

		GX_Position3f32(0 + pos.x, 0 + pos.y, 0 + pos.z);
		GX_Color1u32(0xffffffff);
		GX_TexCoord2f32(0.0f, 0.0f);

		GX_Position3f32(1 + pos.x, 0 + pos.y, 0 + pos.z);
		GX_Color1u32(0xffffffff);
		GX_TexCoord2f32(1.0f, 0.0f);

		GX_Position3f32(1 + pos.x, 1 + pos.y, 0 + pos.z);
		GX_Color1u32(0xffffffff);
		GX_TexCoord2f32(1.0f, 1.0f);

		GX_End();
	}

private:
	RenderMesh3D* _renderMesh;
};

#endif // !ENTITY_TEST_ENTITY_H_
