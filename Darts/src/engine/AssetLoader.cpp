#include "engine/AssetLoader.h"
#include "engine/StackBuffer.h"
#include "engine/Engine.h"
#include "engine/TextureHandle.h"
#include "engine/RenderMeshHandle.h"
#include <fstream>
#include <stdio.h>

enum AssetTypeEnum
{
	AT_INVALID = 0,
	AT_TEXTURE = 1,
	AT_MESH = 2,
};

struct AssetHeader {
	uint16_t assetHeaderVersion;
	uint16_t assetType;
	uint32_t fileSize;
	union TypedAssetData {
		struct ImageData
		{
			uint16_t width;
			uint16_t height;
			uint8_t padding[4];
		} texture;

		struct MeshData
		{
			uint16_t vertexCount;
			uint16_t indexCount;
			uint8_t formatFlags;
			uint8_t vertexSize;
			uint8_t padding[2];
		} mesh;
	} assetData;
};

void* LoadRawAsset(const char* filepath, AssetTypeEnum assetType, AssetHeader& assetHeader) {
	std::ifstream file(filepath, std::ios::binary);
	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	if (fileSize <= sizeof(AssetHeader)) {
		Engine::Log("Failed to load asset, Invalid file size");
		return nullptr;
	}

	file.read((char*)&assetHeader, sizeof(AssetHeader));
	if (!file) {
		Engine::Log("Failed to load asset header, File error bits set");
		return nullptr;
	}

	if (assetHeader.fileSize != fileSize) {
		StackBuffer charBuf(64);
		char* buf = (char*)charBuf.buffer;
		Engine::Log("Failed to load asset, file size and header file size mismatch");
		sprintf(buf, "\tFile Size %i\n\tHeader File Size: %i", fileSize, assetHeader.fileSize);
		Engine::Log(buf);
		return nullptr;
	}

	if (assetHeader.assetHeaderVersion != 0x01) {
		Engine::Log("Failed to load asset, asset header version invalid");
		return nullptr;
	}

	if (assetHeader.assetType != assetType) {
		Engine::Log("Failed to load asset, asset header type and expected type mismatch");
		return nullptr;
	}

	void* assetDataBuffer;
	switch (assetType)
	{
	case AT_TEXTURE:
		assetDataBuffer = Engine::AllocateAlignedSceneMem(fileSize - sizeof(AssetHeader));
		break;
	default:
		assetDataBuffer = Engine::AllocateSceneMem(fileSize - sizeof(AssetHeader));
		break;
	}

	if (!assetDataBuffer) {
		Engine::Log("Failed to load asset, failed to allocate asset buffer");
		return nullptr;
	}

	file.read((char*)assetDataBuffer, fileSize - sizeof(AssetHeader));
	if (!file) {
		Engine::Log("Failed to load asset payload, File error bits set");
		return nullptr;
	}

	return assetDataBuffer;
}

TextureHandle* AssetLoader::LoadTexture(const char* filepath, bool repeat, bool antialias)
{
	AssetHeader header;
	void* rawData = LoadRawAsset(filepath, AT_TEXTURE, header);
	if (!rawData) return nullptr;
	TextureHandle* texture = (TextureHandle*)Engine::AllocateSceneMem(sizeof(TextureHandle));
	if (!texture) {
		Engine::Log("Failed to load texture asset, Failed to allocate TextureHandle");
		return nullptr;
	}

	texture->Init(rawData, header.assetData.texture.width, header.assetData.texture.height, repeat, antialias);
	return texture;
}

RenderMeshHandle* AssetLoader::LoadMesh(const char* filepath)
{
	AssetHeader header;
	void* rawData = LoadRawAsset(filepath, AT_MESH, header);
	if (!rawData) return nullptr;
	RenderMeshHandle* mesh = (RenderMeshHandle*)Engine::AllocateSceneMem(sizeof(RenderMeshHandle));
	if (!mesh) {
		Engine::Log("Failed to load mesh asset, Failed to allocate RenderMeshHandle");
		return nullptr;
	}

	uint8_t* vertexData = (uint8_t*)rawData;
	vertexData += header.assetData.mesh.vertexCount * header.assetData.mesh.vertexSize;
	mesh->Init(header.assetData.mesh.formatFlags, 
		rawData, header.assetData.mesh.vertexCount, 
		(uint16_t*)vertexData,
		header.assetData.mesh.indexCount);
	return mesh;
}
