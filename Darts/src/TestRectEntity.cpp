#include "TestRectEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

float TestRectMeshData[18] = {
	0.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.5f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.0f,
};

TestRectEntity::TestRectEntity() : m_mesh(RMF_HAS_VERTEX_POSITION, TestRectMeshData, 6)
{
	//size_t fileSize = 0;
	//uint8_t* textureFile = Engine::ReadFile("./assets/test.img", &fileSize);
	//if (textureFile == nullptr) {
	//	Engine::GetRenderer().SetClearColor(0xff0000ff);
	//} else if (fileSize == 0) {
	//	Engine::GetRenderer().SetClearColor(0x00ff00ff);
	//}
	//else {
	//	uint16_t width = ((uint16_t)textureFile[0] << 8) + textureFile[1];
	//	uint16_t height = ((uint16_t)textureFile[2] << 8) + textureFile[3];
	//	if (width != 1024) {
	//		Engine::GetRenderer().SetClearColor(0x0000ffff);
	//	}
	//	else if (height != 1024) {
	//		Engine::GetRenderer().SetClearColor(0x0000ffff);
	//	}
	//}
}

TestRectEntity::~TestRectEntity()
{
}

void TestRectEntity::OnRender() {
	m_mesh.Render();
}
