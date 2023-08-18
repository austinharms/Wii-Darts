#include "TestRectEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include "engine/GUI.h"

float TestRectMeshData[18] = {
	0.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.5f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 
};

TestRectEntity::TestRectEntity() {}

TestRectEntity::~TestRectEntity() {}

void TestRectEntity::OnLoad() {
	m_mesh.Init(RMF_HAS_VERTEX_POSITION, TestRectMeshData, 6);
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
	m_mesh.Render();
}
