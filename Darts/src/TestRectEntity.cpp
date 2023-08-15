#include "TestRectEntity.h"
#include "Engine/Engine.h"

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
}

TestRectEntity::~TestRectEntity()
{
}

void TestRectEntity::OnRender() {
	m_mesh.Render();
}
