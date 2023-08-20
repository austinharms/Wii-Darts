#include "engine/RenderMeshHandle.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

RenderMeshHandle::RenderMeshHandle() { Reset(); }

RenderMeshHandle::~RenderMeshHandle() {}

void RenderMeshHandle::Render(TextureHandle* texture, uint32_t color)
{
	Engine::GetRenderer().DrawRenderMesh(*this, texture, color);
}

bool RenderMeshHandle::Init(RenderMeshFormat format, void* vertexData, uint16_t vertexCount, uint16_t* indexData, uint16_t indexCount) {
	Reset();
	m_format = format;
	m_vertexData = vertexData;
	m_vertexCount = vertexCount;
	m_indexData = indexData;
	m_indexCount = indexCount;
	return GetValid();
}

WD_NODISCARD RenderMeshFormat RenderMeshHandle::GetFormat() const
{
	return m_format;
}

WD_NODISCARD const void* RenderMeshHandle::GetVertexBuffer() const
{
	return m_vertexData;
}

WD_NODISCARD uint16_t RenderMeshHandle::GetVertexCount() const
{
	return m_vertexCount;
}

WD_NODISCARD const uint16_t* RenderMeshHandle::GetIndexBuffer() const
{
	return m_indexData;
}

WD_NODISCARD uint16_t RenderMeshHandle::GetIndexCount() const
{
	return m_indexCount;
}

WD_NODISCARD bool RenderMeshHandle::GetValid() const
{
	return m_vertexData && m_vertexCount && ((m_format & RMF_HAS_INDICES) == 0 || (m_indexData && m_indexCount && m_indexCount % 3 == 0));
}

void RenderMeshHandle::Reset() {
	m_indexData = nullptr;
	m_vertexData = nullptr;
	m_indexCount = 0;
	m_vertexCount = 0;
	m_format = 0;
}

