#include "DartThrowUI.h"
#include "engine/GUI.h"
#include "engine/Engine.h"
#include "engine/Input.h"
#include "engine/CameraEntity.h"
#include "engine/moreFMath.h"
#include "DartEntity.h"

DartThrowUI::DartThrowUI() {
	m_strength = 0;
	m_swing = 0;
}

DartThrowUI::~DartThrowUI()
{
}

void DartThrowUI::SetThrowPower(float strength, float swing)
{
	m_strength = strength;
	m_swing = swing;
}

void DartThrowUI::OnRender() {
	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize;
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 size(io.DisplaySize.x / 6, io.DisplaySize.y / 4);
	ImGui::SetNextWindowPos(io.DisplaySize - size - ImVec2(20, 50));
	ImGui::SetNextWindowSize(size);
	ImGui::Begin("Throw Power UI", nullptr, flags);
	ImDrawList* list = ImGui::GetForegroundDrawList();
	ImVec2 c = ImGui::GetCursorScreenPos();
	float center = (size.x / 2) + c.x;
	ImVec2 centerPos(center, c.y + (size.y / 2));
	list->AddBezierQuadratic(ImVec2(center, c.y), centerPos, ImVec2(center, c.y + size.y), IM_COL32(0x57, 0x57, 0x57, 0xFF), 15, 0);
	list->AddBezierQuadratic(ImVec2(center, c.y), centerPos, ImVec2(center, c.y + size.y), IM_COL32(0x8C, 0x8C, 0x8C, 0xFF), 10, 0);
	list->AddTriangleFilled(centerPos + ImVec2(-10, 10), centerPos + ImVec2(-10, -10), centerPos + ImVec2(-3, 0), IM_COL32(0x37, 0x37, 0x37, 0xFF));
	list->AddTriangleFilled(centerPos + ImVec2(10, 10), centerPos + ImVec2(10, -10), centerPos + ImVec2(3, 0), IM_COL32(0x37, 0x37, 0x37, 0xFF));

	float power = (size.y / 2) + (m_strength * (size.y/2));
	float swing = (size.x / 2) + (m_swing * (size.x/2));
	list->AddBezierQuadratic(ImVec2(center, c.y + size.y), ImVec2(center, c.y + size.y - (power/2)), ImVec2(c.x + swing, c.y + size.y - power), IM_COL32(0xFF, 0x60, 0x60, 0xFF), 10, 10);
	ImGui::Dummy(size);
	ImGui::End();

	ImGui::Begin("Throw Stats");
	ImGui::Text("Power: %f, Swing: %f", m_strength, m_swing);
	ImGui::End();
}
