#include "DartEntity.h"
#include "engine/AssetLoader.h"

DartEntity::DartEntity() {
	m_dartCoreMesh = nullptr;
	m_dartFinsMesh = nullptr;
}

DartEntity::~DartEntity() {

}

void DartEntity::OnLoad() {
	m_dartCoreMesh = AssetLoader::LoadMesh("./assets/dart_core.mesh");
	m_dartFinsMesh = AssetLoader::LoadMesh("./assets/dart_fins.mesh");
}

void DartEntity::OnRender() {
	GetTransform().Rotate({ 0, 0.01f, 0 });
	MarkTransformDirty();
	m_dartCoreMesh->Render(nullptr, 0xedededff);
	m_dartFinsMesh->Render(nullptr, 0x00eeffff);
}