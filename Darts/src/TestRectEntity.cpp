#include "TestRectEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include "engine/GUI.h"
#include "engine/AssetLoader.h"

float TestRectMeshData[30] = {
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	0.5f, 0.5f, 0.0f,  1.0f, 1.0f,
	0.0f, 0.5f, 0.0f,  0.0f, 1.0f,
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	0.5f, 0.0f, 0.0f,  1.0f, 0.0f,
	0.5f, 0.5f, 0.0f,  1.0f, 1.0f,
};

TestRectEntity::TestRectEntity() { 
	m_texture = nullptr;
	m_mesh = nullptr;
}

TestRectEntity::~TestRectEntity() {}

void TestRectEntity::OnLoad() {
	//m_mesh.Init(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_UVS, TestRectMeshData, 6);
	m_mesh = AssetLoader::LoadMesh("./assets/dart.mesh");
	m_texture = AssetLoader::LoadTexture("./assets/test.img");
}

void TestRectEntity::OnRender() {
	gui::Begin("Main Args");
	int argc;
	char** argv;
	Engine::GetMainArgs(&argc, &argv);
	for (int i = 0; i < argc; i++)
		gui::Text(argv[i]);
	gui::End();
	gui::ShowDemoWindow();
	m_mesh->Render(m_texture);
}
