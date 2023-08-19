#ifndef DARTS_ASSET_LOADER_H_
#define DARTS_ASSET_LOADER_H_
#include "Core.h"

class TextureHandle;
class RenderMeshHandle;

// Class for loading assets from file
// All loaded assets are invalidated when the RootEntity is changed
class AssetLoader
{
public:
	// Loads a texture from filepath or nullptr on error
	static TextureHandle* LoadTexture(const char* filepath, bool repeat = false, bool antialias = false);

	// Loads a render mesh from filepath or nullptr on error
	static RenderMeshHandle* LoadMesh(const char* filepath);
	
private:
	AssetLoader();
	~AssetLoader();
};
#endif // !DARTS_ASSET_LOADER_H_
