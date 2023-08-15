#include "RenderMesh.h"
#include "EngineCore.h"
#include "Renderer.h"

RenderMesh::RenderMesh(RenderMeshFormat format, void* vertexData, uint16_t vertexCount, uint16_t* indexData, uint16_t indexCount) : m_indexData(indexData), m_vertexData(vertexData), m_indexCount(indexCount), m_vertexCount(vertexCount), m_format(format) { }

RenderMesh::~RenderMesh() {

}

void RenderMesh::Render(TextureHandle* texture)
{
	EngineCore::GetRenderer().DrawRenderMesh(*this, texture);
}

WD_NODISCARD RenderMeshFormat RenderMesh::GetFormat() const
{
	return m_format;
}

WD_NODISCARD const void* RenderMesh::GetVertexBuffer() const
{
	return m_vertexData;
}

WD_NODISCARD uint16_t RenderMesh::GetVertexCount() const
{
	return m_vertexCount;
}

WD_NODISCARD const uint16_t* RenderMesh::GetIndexBuffer() const
{
	return m_indexData;
}

WD_NODISCARD uint16_t RenderMesh::GetIndexCount() const
{
	return m_indexCount;
}

WD_NODISCARD bool RenderMesh::GetValid() const
{
	return m_vertexData && m_vertexCount && ((m_format & RMF_HAS_INDICES) == 0 || m_indexData && m_indexCount && m_indexCount % 3 == 0);
}



