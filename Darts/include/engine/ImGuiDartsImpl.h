#ifndef DARTS_IMGUI_IMPL_H_
#define DARTS_IMGUI_IMPL_H_
#include "TextureHandle.h"

class ImGuiDartsImpl {
public:
	ImGuiDartsImpl();
	~ImGuiDartsImpl();
	void Init();
	void UpdateFontAtlas();
	void StartFrame();
	void RenderUI();
	TextureHandle* GetFontAtlasTexture();

private:
	TextureHandle m_fontAtlasTexture;
	bool m_init;
	bool m_buildFontAtlas;
};
#endif // !DARTS_IMGUI_IMPL_H_
