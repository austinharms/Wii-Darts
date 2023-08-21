#ifndef DARTS_ROOM_ENTITY_H_
#define DARTS_ROOM_ENTITY_H_
#include "engine/Core.h"
#include "engine/Entity.h"
#include "engine/TextureHandle.h"
#include "engine/RenderMeshHandle.h"

class RoomEntity : public Entity
{
public:
	RoomEntity();
	virtual ~RoomEntity();
	void OnLoad() WD_OVERRIDE;
	void OnRender() WD_OVERRIDE;

private:
	RenderMeshHandle* m_mesh;
	TextureHandle* m_texture;
};
#endif // !DARTS_ROOM_ENTITY_H_
