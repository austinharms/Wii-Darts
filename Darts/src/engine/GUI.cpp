#include "engine/GUI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "engine/RenderMeshHandle.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include "cursor_color_img.h"
#include "cursor_top_img.h"
#include <stdio.h>
#include <cassert>

void ImGuiAssertCallback(const char* msg) {
	Engine::Log(msg);
	assert(false);
}

GUI::GUI()
{
	IMGUI_CHECKVERSION();
	m_init = false;
	m_showCursors = true;
	m_cursorForgroundIndex = 0;
	m_cursorBackgroundIndex = 0;
}

GUI::~GUI()
{
	if (m_init) ImGui::DestroyContext();
}

void GUI::ShowCursors(bool show)
{
	m_showCursors = show;
}

WD_NODISCARD bool GUI::GetUsingInput() const
{
	return ImGui::GetIO().WantCaptureMouse;
}

void GUI::Init() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.BackendRendererName = "ImGuiDartsImpl";
	io.BackendRendererUserData = this;
	io.BackendPlatformName = "ImGuiDartsImpl";
	io.BackendPlatformUserData = this;
	io.IniFilename = "sd:/imgui.ini";

	uint16_t w, h;
	Engine::GetRenderer().GetFramebufferSize(&w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(1, 1);
	io.DeltaTime = (1.0f / 60.0f);
	io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);

	io.MouseDrawCursor = false;
	ImGui::StyleColorsDark();
	m_init = true;
}

void GUI::UpdateFontAtlas()
{
	uint32_t* pixelData = nullptr;
	int w, h;
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	ImFontConfig fontCfg;
	fontCfg.SizePixels = 35;
	fontCfg.OversampleH = 3;
	fontCfg.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
	io.Fonts->AddFontDefault(&fontCfg);
	io.Fonts->TexDesiredWidth = 512;
	m_cursorForgroundIndex = io.Fonts->AddCustomRectRegular(64, 64);
	m_cursorBackgroundIndex = io.Fonts->AddCustomRectRegular(64, 64);
	//io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
	io.Fonts->Build();
	if (!io.Fonts->TexReady) {
		Engine::Log("Failed to build font atlas");
		Engine::Quit();
		return;
	}

	io.Fonts->GetTexDataAsRGBA32((unsigned char**)&pixelData, &w, &h);
	PackCursorTexture(pixelData, w);
	size_t pixelBufferSize = w * h * 4;
	void* textureBuffer = Engine::AllocateAlignedSceneMem(pixelBufferSize);
	if (!textureBuffer) Engine::Log("Failed to allocate texture atlas mem");
	TextureHandle::ConvertRGBA8PixelData((uint8_t*)textureBuffer, pixelData, w, h);
	io.Fonts->ClearTexData();
	m_fontAtlasTexture.Init(textureBuffer, w, h, false, true);
	io.Fonts->SetTexID(&m_fontAtlasTexture);
	io.FontGlobalScale = 0.5;
	StackBuffer stackBuf(64);
	sprintf((char*)stackBuf.buffer, "Updated Font Atlas, Size %i X %i", w, h);
	Engine::Log((char*)stackBuf.buffer);
}

void GUI::ProcessEvents()
{
	ImGuiIO& io = ImGui::GetIO();
	Input& input = Engine::GetInput();
	io.DeltaTime = Engine::GetDelta();

	float x = -FLT_MAX;
	float y = -FLT_MAX;
	input.GetIRScreenPose(&x, &y);
	io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
	io.AddMousePosEvent(x, y);

	if (input.GetButtonPressed(WPAD_BUTTON_A))
		io.AddMouseButtonEvent(0, true);
	if (input.GetButtonPressed(WPAD_BUTTON_B))
		io.AddMouseButtonEvent(1, true);
	if (input.GetButtonReleased(WPAD_BUTTON_A))
		io.AddMouseButtonEvent(0, false);
	if (input.GetButtonReleased(WPAD_BUTTON_B))
		io.AddMouseButtonEvent(1, false);
}

void GUI::StartFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	uint16_t w, h;
	Engine::GetRenderer().GetFramebufferSize(&w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	ImGui::NewFrame();
}

void GUI::RenderUI()
{
	if (m_showCursors || GetUsingInput()) DrawCursors();
	ImGui::Render();
	Renderer& renderer = Engine::GetRenderer();
	ImDrawData* drawData = ImGui::GetDrawData();
	RenderMeshHandle mesh;
	renderer.ResetScissor();
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];
		// Clear Z Component of vertex positions
		ImDrawVert* vertexItr = cmdList->VtxBuffer.Data;
		for (uint16_t i = 0; i < cmdList->VtxBuffer.Size; ++i) {
			vertexItr[i].posZ = 0;
		}

		for (int cmdI = 0; cmdI < cmdList->CmdBuffer.Size; cmdI++) {
			const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdI];
			if (pcmd->UserCallback != nullptr)
			{
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
					renderer.ResetScissor();
				else
					pcmd->UserCallback(cmdList, pcmd);
			}
			else
			{
				const ImVec4& clipRect = pcmd->ClipRect;
				if (clipRect.z <= clipRect.x || clipRect.w <= clipRect.y)
					continue;

				renderer.SetScissor(clipRect.x, clipRect.y, clipRect.z - clipRect.x, clipRect.w - clipRect.y);
				mesh.Init(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_COLOR | RMF_HAS_VERTEX_UVS | RMF_HAS_INDICES, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size, &(cmdList->IdxBuffer.Data[pcmd->IdxOffset]), pcmd->ElemCount);
				renderer.DrawRenderMesh(mesh, (TextureHandle*)(pcmd->GetTexID()), 0xffffffff, WD_RENDER_UI);
			}
		}
	}
}

void GUI::DrawCursors() {
	static constexpr uint32_t colors[8]{
		0x000090FF,
		0x900000FF,
		0x009000FF,
		0x909000FF,
		0x00009090,
		0x90000090,
		0x00900090,
		0x90900090,
	};

	static constexpr const char* strings[4]{
		"1",
		"2",
		"3",
		"4",
	};

	Input& input = Engine::GetInput();
	ImVec2 cursorPos;
	for (int i = 0; i < 4; ++i) {
		if (input.GetActiveController() == i) continue;
		if (input.GetIRScreenPose(i, &cursorPos.x, &cursorPos.y)) {
			cursorPos.x += -11;
			cursorPos.y += -2;
			DrawCursor(cursorPos, colors[i], strings[i]);
		}
	}

	// Draw active cursor last so its on top
	if (input.GetIRScreenPose(input.GetActiveController(), &cursorPos.x, &cursorPos.y)) {
		cursorPos.x += -11;
		cursorPos.y += -2;
		DrawCursor(cursorPos, colors[input.GetActiveController()], strings[input.GetActiveController()]);
	}
}

void GUI::DrawCursor(const ImVec2& posTL, uint32_t color, const char* text)
{
	ImGuiContext& g = *ImGui::GetCurrentContext();
	ImFontAtlas* font_atlas = g.DrawListSharedData.Font->ContainerAtlas;

	ImVec2 colorMin;
	ImVec2 colorMax;
	ImVec2 cursorMin;
	ImVec2 cursorMax;
	constexpr ImVec2 cursorSize(32, 32);
	constexpr ImVec2 cursorTextOffset(12, 12);
	const ImVec2 posBR(posTL.x + cursorSize.x, posTL.y + cursorSize.y);

	font_atlas->CalcCustomRectUV(font_atlas->GetCustomRectByIndex(m_cursorBackgroundIndex), &colorMin, &colorMax);
	font_atlas->CalcCustomRectUV(font_atlas->GetCustomRectByIndex(m_cursorForgroundIndex), &cursorMin, &cursorMax);
	for (int n = 0; n < g.Viewports.Size; n++)
	{
		ImGuiViewportP* viewport = g.Viewports[n];
		if (!viewport->GetMainRect().Overlaps(ImRect(posTL, posBR)))
			continue;
		ImDrawList* draw_list = ImGui::GetForegroundDrawList(viewport);
		ImTextureID tex_id = font_atlas->TexID;
		draw_list->PushTextureID(tex_id);
		draw_list->AddImage(tex_id, posTL, posBR, colorMin, colorMax, color);
		draw_list->AddImage(tex_id, posTL, posBR, cursorMin, cursorMax, 0xFFFFFF00 | (uint8_t)color);
		draw_list->PopTextureID();
		draw_list->AddText(ImVec2(posTL.x + cursorTextOffset.x, posTL.y + cursorTextOffset.y), color, text);
	}
}

void GUI::PackCursorTexture(uint32_t* pixels, int width)
{
	ImGuiIO& io = ImGui::GetIO();
	//cursor_top_img
	//cursor_color_img
	StackBuffer imgStackBuffer(16384);
	uint32_t* imgBuffer = (uint32_t*)imgStackBuffer.buffer;
	GetTextureData(cursor_top_img, imgBuffer);
	ImFontAtlasCustomRect* rect = io.Fonts->GetCustomRectByIndex(m_cursorForgroundIndex);
	for (int y = 0; y < 64; ++y) {
		for (int x = 0; x < 64; ++x) {
			pixels[x + rect->X + ((y + rect->Y) * width)] = imgBuffer[x + (y * 64)];
		}
	}

	rect = io.Fonts->GetCustomRectByIndex(m_cursorBackgroundIndex);
	GetTextureData(cursor_color_img, imgBuffer);
	for (int y = 0; y < 64; ++y) {
		for (int x = 0; x < 64; ++x) {
			pixels[x + rect->X + ((y + rect->Y) * width)] = imgBuffer[x + (y * 64)];
		}
	}
}

// Converts the packed data back into RGBA8
void GUI::GetTextureData(const uint8_t* fileBytes, uint32_t* dstBuffer)
{
	fileBytes += 16;
	int j = 0;
	for (int y = 0; y < 64; y += 4) {
		for (int x = 0; x < 64; x += 4) {
			uint32_t subData[16];
			for (int i = 0; i < 16; ++i) {
				subData[i] = *fileBytes;
				fileBytes += 1;
				subData[i] |= (uint32_t)(*fileBytes) << 24;
				fileBytes += 1;
			}

			j -= 32;
			for (int i = 0; i < 16; ++i) {
				subData[i] |= (uint32_t)(*fileBytes) << 16;
				fileBytes += 1;
				subData[i] |= (uint32_t)(*fileBytes) << 8;
				fileBytes += 1;
			}

			for (int y2 = 0; y2 < 4; y2++) {
				for (int x2 = 0; x2 < 4; x2++) {
					dstBuffer[x + x2 + ((y + y2) * 64)] = subData[x2 + (y2 * 4)];
				}
			}
		}
	}
}
