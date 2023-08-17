#include "TestRectEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include "test_img.h"

float TestRectMeshData[30] = {
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	0.5f, 0.5f, 0.0f,  1.0f, 1.0f,
	0.0f, 0.5f, 0.0f,  0.0f, 1.0f,
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	0.5f, 0.0f, 0.0f,  1.0f, 0.0f,
	0.5f, 0.5f, 0.0f,  1.0f, 1.0f,
};

TestRectEntity::TestRectEntity() : m_mesh(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_UVS, TestRectMeshData, 6), m_texture(((uint8_t*)test_img) + 4, 1024, 1024)
{
	//size_t fileSize = 0;
	//uint8_t* textureFile = Engine::ReadFile("./assets/test.img", &fileSize);	
}

TestRectEntity::~TestRectEntity()
{
}

void TestRectEntity::OnRender() {
	m_mesh.Render(&m_texture);
}
