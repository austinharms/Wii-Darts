#include "TestRectEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include "engine/GUI.h"

float TestRectMeshData[30] = {
	0.0f, 0.0f, 0.0f,  0.478516f, 0.0f,
	0.5f, 0.5f, 0.0f,  0.498047f, 0.156250f,
	0.0f, 0.5f, 0.0f,  0.478516f, 0.156250f,
	0.0f, 0.0f, 0.0f,  0.478516f, 0.0f,
	0.5f, 0.0f, 0.0f,  0.498047f, 0.0f,
	0.5f, 0.5f, 0.0f,  0.498047f, 0.156250f,
};

TestRectEntity::TestRectEntity() : m_mesh(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_UVS, TestRectMeshData, 6)
{
	//size_t fileSize = 0;
	//uint8_t* textureFile = Engine::ReadFile("./assets/test.img", &fileSize);	
}

TestRectEntity::~TestRectEntity()
{
}

void TestRectEntity::OnRender() {
	gui::ShowDemoWindow();
	//m_mesh.Render();
}
