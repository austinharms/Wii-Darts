#include "engine/ImGuiDartsImpl.h"
#include "imgui.h"
#include "engine/RenderMesh.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <new>
#include <stdio.h>

ImGuiDartsImpl::ImGuiDartsImpl() : m_fontAtlasTexture(nullptr, 0, 0)
{
	IMGUI_CHECKVERSION();
	m_init = false;
	m_buildFontAtlas = false;
}

ImGuiDartsImpl::~ImGuiDartsImpl()
{
	if (m_init) ImGui::DestroyContext();
}

void ImGuiDartsImpl::Init() {
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

void ImGuiDartsImpl::UpdateFontAtlas()
{
	m_fontAtlasTexture.~TextureHandle();
	unsigned char* pixelData = nullptr;
	int w, h;
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	ImFontConfig fontCfg;
	fontCfg.SizePixels = 15;
	fontCfg.OversampleH = 2;
	fontCfg.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
	io.Fonts->AddFontDefault(&fontCfg);
	io.Fonts->TexDesiredWidth = 256;
	//io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
	io.Fonts->Build();
	if (!io.Fonts->TexReady) {
		Engine::Log("Failed to build font atlas");
		Engine::Quit();
	}

	io.Fonts->GetTexDataAsRGBA32(&pixelData, &w, &h);
	new((void*)&m_fontAtlasTexture) TextureHandle(pixelData, w, h, false, false, WD_PIXEL_RGBA8);
	io.Fonts->SetTexID(&m_fontAtlasTexture);
	io.FontGlobalScale = 1;

	char* buf = (char*)Engine::AllocateTempMem(512);
	sprintf(buf, "Updated Font Atlas, Size %i X %i", w, h);
	Engine::Log(buf);
	m_buildFontAtlas = true;
}

void ImGuiDartsImpl::StartFrame()
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

void ImGuiDartsImpl::RenderUI()
{
	ImGui::ShowDemoWindow();
	ImGui::Render();
	Renderer& renderer = Engine::GetRenderer();
	ImDrawData* drawData = ImGui::GetDrawData();
	ImVec2 clip_off = drawData->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
	renderer.ResetScissor();
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmdList = drawData->CmdLists[n];

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
				ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
				ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
				if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					continue;

				renderer.SetScissor(clip_min.x, clip_min.y, clip_max.x - clip_min.x, clip_max.y - clip_min.y);
				RenderMesh mesh(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_COLOR | RMF_HAS_VERTEX_UVS | RMF_HAS_INDICES, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size, &(cmdList->IdxBuffer.Data[pcmd->IdxOffset]), pcmd->ElemCount);
				// Clear Z Component of positions
				ImDrawVert* vertexItr = cmdList->VtxBuffer.Data;
				for (uint16_t i = 0; i < cmdList->VtxBuffer.Size; ++i) {
					vertexItr[i].posZ = 0;
				}

				renderer.DrawRenderMesh(mesh, (TextureHandle*)(pcmd->GetTexID()), WD_RENDER_UI);
				
				//// Project scissor/clipping rectangles into framebuffer space
				//const ImVec4& rect = pcmd->ClipRect;
				//if (rect.z <= rect.x || rect.w <= rect.y)
				//	continue;

				////char* buffer = (char*)Engine::AllocateTempMem(5000);
				////sprintf(buffer, "Render Count: %i, Vtx: %f %f %f", pcmd->ElemCount, ((float*)cmdList->VtxBuffer.Data)[0], ((float*)cmdList->VtxBuffer.Data)[1], ((float*)cmdList->VtxBuffer.Data)[2]);
				////Engine::Log(buffer);
				//renderer.SetScissor(rect.x, rect.y, rect.z - rect.x, rect.w - rect.y);
				//RenderMesh mesh(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_COLOR | RMF_HAS_VERTEX_UVS | RMF_HAS_INDICES, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size, &(cmdList->IdxBuffer.Data[pcmd->IdxOffset]), pcmd->ElemCount);
				//
				//// Clear Z Component of positions
				//ImDrawVert* vertexItr = cmdList->VtxBuffer.Data;
				//for (uint16_t i = 0; i < mesh.GetVertexCount(); ++i) {
				//	vertexItr[i].posZ = 0;
				//}

				//renderer.DrawRenderMesh(mesh, ((TextureHandle*)pcmd->GetTexID()), WD_RENDER_UI);
			}
		}

		//RenderMesh mesh(RMF_HAS_VERTEX_POSITION | RMF_HAS_VERTEX_COLOR | RMF_HAS_VERTEX_UVS | RMF_HAS_INDICES, cmdList->VtxBuffer.Data,cmdList->VtxBuffer.Size, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size);
	}
}

TextureHandle* ImGuiDartsImpl::GetFontAtlasTexture()
{
	return &m_fontAtlasTexture;
}
