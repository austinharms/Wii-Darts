#include "RoomEntity.h"
#include "engine/AssetLoader.h"

RoomEntity::RoomEntity() {
	m_mesh = nullptr;
	m_texture = nullptr;
}

RoomEntity::~RoomEntity() {

}

void RoomEntity::OnLoad() {
	m_mesh = AssetLoader::LoadMesh("./assets/room.mesh");
	m_texture = AssetLoader::LoadTexture("./assets/room.img");
	//GetTransform().SetPosition((guVector) { 0, 5, 0 });
	//MarkTransformDirty();
}

void RoomEntity::OnRender() {
	m_mesh->Render(m_texture);
}