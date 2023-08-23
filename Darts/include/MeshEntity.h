#ifndef DARTS_MESH_ENTITY_H_
#define DARTS_MESH_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"
#include "engine//RenderMeshHandle.h"
#include "engine/TextureHandle.h"

class MeshEntity : public Entity 
{
public:
	MeshEntity(const char* meshFilepath, const char* textureFilepath = nullptr);
	virtual ~MeshEntity();
	using Entity::GetTransform;
	using Entity::MarkTransformDirty;

protected:
	void OnLoad() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;

private:
	const char* m_meshFile;
	const char* m_textureFile;
	RenderMeshHandle* m_mesh;
	TextureHandle* m_texture;
};
#endif // !DARTS_MESH_ENTITY_H_
