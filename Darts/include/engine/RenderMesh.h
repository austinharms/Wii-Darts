#ifndef DARTS_RENDER_MESH_H_
#define DARTS_RENDER_MESH_H_
#include "Core.h"

class TextureHandle;

typedef uint8_t RenderMeshFormat;
enum RenderMeshFormatBits : RenderMeshFormat
{
	RMF_HAS_VERTEX_POSITION = 0, // All meshes require vertex positions
	RMF_HAS_VERTEX_NORMAL = 0b00000001,
	RMF_HAS_VERTEX_COLOR = 0b00000010,
	RMF_HAS_VERTEX_UVS = 0b00000100,
	RMF_HAS_INDICES = 0b00001000,
};

class RenderMesh
{
public:
	// The RenderMesh will assume the buffers exist for the whole lifetime of the RenderMesh object
	RenderMesh(RenderMeshFormat format, void* vertexData, uint16_t vertexCount, uint16_t* indexData = nullptr, uint16_t indexCount = 0);
	~RenderMesh();
	WD_NOCOPY(RenderMesh);
	void Render(TextureHandle* texture = nullptr);
	WD_NODISCARD RenderMeshFormat GetFormat() const;
	WD_NODISCARD const void* GetVertexBuffer() const;
	WD_NODISCARD uint16_t GetVertexCount() const;
	WD_NODISCARD const uint16_t* GetIndexBuffer() const;
	WD_NODISCARD uint16_t GetIndexCount() const;
	WD_NODISCARD bool GetValid() const;

private:
	const uint16_t* m_indexData;
	const void* m_vertexData;
	const uint16_t m_indexCount;
	const uint16_t m_vertexCount;
	const RenderMeshFormat m_format;
};
#endif // !DARTS_RENDER_MESH_H_
