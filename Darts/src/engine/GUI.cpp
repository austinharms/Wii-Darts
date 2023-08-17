#include "engine/GUI.h"
#include "imgui.h"
#include "engine/RenderMesh.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <new>
#include <stdio.h>
#include <cassert>

void ImGuiAssertCallback(const char* msg) {
	Engine::Log(msg);
	assert(false);
}

GUI::GUI() : m_fontAtlasTexture(nullptr, 0, 0)
{
	IMGUI_CHECKVERSION();
	m_init = false;
	m_buildFontAtlas = false;
}

GUI::~GUI()
{
	if (m_init) ImGui::DestroyContext();
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

	io.MouseDrawCursor = true;
	ImGui::StyleColorsDark();
	m_init = true;
}

void GUI::UpdateFontAtlas()
{
	m_fontAtlasTexture.~TextureHandle();
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
	//io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
	io.Fonts->Build();
	if (!io.Fonts->TexReady) {
		Engine::Log("Failed to build font atlas");
		Engine::Quit();
	}

	io.Fonts->GetTexDataAsRGBA32((unsigned char**)&pixelData, &w, &h);
	int pixelCount = w * h;
	new((void*)&m_fontAtlasTexture) TextureHandle(pixelData, w, h, false, true, WD_PIXEL_RGBA8);
	io.Fonts->SetTexID(&m_fontAtlasTexture);
	io.FontGlobalScale = 0.5;
	char* buf = (char*)Engine::AllocateTempMem(512);
	sprintf(buf, "Updated Font Atlas, Size %i X %i", w, h);
	Engine::Log(buf);
	m_buildFontAtlas = true;
}

void GUI::StartFrame()
{
	if (!m_buildFontAtlas) 	UpdateFontAtlas();

	ImGuiIO& io = ImGui::GetIO();
	Input& input = Engine::GetInput();

	uint16_t w, h;
	Engine::GetRenderer().GetFramebufferSize(&w, &h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DeltaTime = Engine::GetDelta();

	float x = -FLT_MAX;
	float y = -FLT_MAX;
	input.GetControllerIRScreenPos(0, &x, &y);
	io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
	io.AddMousePosEvent(x, y);

	if (input.GetControllerButtonPressed(0, WPAD_BUTTON_A))
		io.AddMouseButtonEvent(0, true);
	if (input.GetControllerButtonPressed(0, WPAD_BUTTON_B))
		io.AddMouseButtonEvent(1, true);
	if (input.GetControllerButtonReleased(0, WPAD_BUTTON_A))
		io.AddMouseButtonEvent(0, false);
	if (input.GetControllerButtonReleased(0, WPAD_BUTTON_B))
		io.AddMouseButtonEvent(1, false);

	ImGui::NewFrame();
}

void GUI::RenderUI()
{
	ImGui::Render();
	Renderer& renderer = Engine::GetRenderer();
	ImDrawData* drawData = ImGui::GetDrawData();
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
				RenderMesh mesh(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_COLOR | RMF_HAS_VERTEX_UVS | RMF_HAS_INDICES, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size, &(cmdList->IdxBuffer.Data[pcmd->IdxOffset]), pcmd->ElemCount);
				renderer.DrawRenderMesh(mesh, (TextureHandle*)(pcmd->GetTexID()), WD_RENDER_UI);
			}
		}
	}
}

