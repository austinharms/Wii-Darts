#ifndef DARTS_IMGUI_IMPL_H_
#define DARTS_IMGUI_IMPL_H_
#include "TextureHandle.h"
#include "imgui.h"

class Engine;
class GUI {
public:
	~GUI();
	void ShowCursors(bool show);
	WD_NODISCARD bool GetUsingInput() const;

private:
	friend class Engine;

	TextureHandle m_fontAtlasTexture;
	int m_cursorForgroundIndex;
	int m_cursorBackgroundIndex;
	bool m_init;
	bool m_showCursors;

	GUI();
	void Init();
	void UpdateFontAtlas();
	void ProcessEvents();
	void StartFrame();
	void RenderUI();
	void DrawCursors();
	void DrawCursor(const ImVec2& pos, uint32_t color, const char* text);
	void PackCursorTexture(uint32_t* pixels, int width);
	void GetTextureData(const uint8_t* fileBytes, uint32_t* dstBuffer);
};
#endif // !DARTS_IMGUI_IMPL_H_
