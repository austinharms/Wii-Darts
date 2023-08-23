#include "MeshEntity.h"
#include "engine/AssetLoader.h"

MeshEntity::MeshEntity(const char* meshFilepath, const char* textureFilepath) : m_meshFile(meshFilepath), m_textureFile(textureFilepath) {
	m_mesh = nullptr;
	m_texture = nullptr;
}

MeshEntity::~MeshEntity() { }

void MeshEntity::OnLoad() {
	m_mesh = AssetLoader::LoadMesh(m_meshFile);
	if (m_textureFile)
		m_texture = AssetLoader::LoadTexture(m_textureFile);
}

void MeshEntity::OnRender() {
	if (m_mesh)	m_mesh->Render(m_texture);
}
