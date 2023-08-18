#ifndef DARTS_IMGUI_IMPL_H_
#define DARTS_IMGUI_IMPL_H_
#include "TextureHandle.h"
#include "imgui.h"

namespace gui = ImGui;

class Engine;
class GUI {
public:
	~GUI();

private:
	friend class Engine;

	TextureHandle m_fontAtlasTexture;
	bool m_init;

	GUI();
	void Init();
	void UpdateFontAtlas();
	void StartFrame();
	void RenderUI();
};
#endif // !DARTS_IMGUI_IMPL_H_
