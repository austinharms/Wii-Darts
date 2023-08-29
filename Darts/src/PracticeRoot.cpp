#include "PracticeRoot.h"
#include "DartEntity.h"
#include "MeshEntity.h"
#include "engine/LightEntity.h"
#include "GUICameraEntity.h"
#include "engine/GUI.h"
#include "DartEntity.h"
#include "MainMenuRoot.h"
#include "engine/Input.h"
#include "engine/Engine.h"
#include <new>

struct PracticeRoot::SceneData {
	float m_speeds[3];
	DartEntity* m_dart;

	SceneData() {
		m_speeds[0] = 0;
		m_speeds[1] = -9;
		m_speeds[2] = -10;
	}
};

PracticeRoot::PracticeRoot() {
	WD_STATIC_ASSERT(sizeof(PracticeRoot::SceneData) <= sizeof(m_userData), "PracticeRoot::SceneData too big");
	new(m_userData) SceneData();
}

PracticeRoot::~PracticeRoot() {
	GetData().~SceneData();
}

void PracticeRoot::OnLoad() {
	Engine::GetGUI().ShowCursors(false);
	// Setup lighting
	{
		LightEntity* roomLight = AddChild<LightEntity>();
		Transform& roomLightTransform = roomLight->GetTransform();
		roomLightTransform.Reset();
		roomLightTransform.SetPosition({ 0,-20,0 });
		roomLightTransform.LookAt({ 0,1,0 }, { 0,0,-1 });
		roomLight->SetColor({ 0xff, 0xff, 0xff, 0xff });
		roomLight->SetSpecularEnabled(false);
		roomLight->SetProperties(90, 30, 0.90);
	}

	{
		LightEntity* roomLight = AddChild<LightEntity>();
		Transform& roomLightTransform = roomLight->GetTransform();
		roomLightTransform.Reset();
		roomLightTransform.SetPosition({ 0,0,10 });
		roomLightTransform.LookAt({ 0,0,0 }, { 0,1,0 });
		roomLight->SetColor({ 0xff, 0xff, 0xff, 0xff });
		roomLight->SetSpecularEnabled(false);
		roomLight->SetProperties(5, 30, 0.99);
	}

	// Setup Meshes 
	AddChild<MeshEntity>("./assets/dense_plane.mesh", "./assets/room.img");
	AddChild<MeshEntity>("./assets/board.mesh", "./assets/board.img");
	GetData().m_dart = AddChild<DartEntity>();
	Transform& cameraTransform = AddChild<GUICameraEntity>()->GetTransform();
	cameraTransform.SetPosition({ 0,0,2.37f });
}

void PracticeRoot::OnRender() {
	SceneData& data = GetData();
	ImGui::Begin("Dart");
	ImGui::DragFloat3("Speed", data.m_speeds, 0.01f, -100, 100);
	if (ImGui::Button("Throw")) {
		data.m_dart->GetTransform().Reset();
		data.m_dart->GetTransform().Translate({ 0,0,5 });
		guVector v = { data.m_speeds[0], data.m_speeds[1], data.m_speeds[2] };
		data.m_dart->Throw(v);
	}

	ImGui::End();
}

void PracticeRoot::OnUpdate() {
	if (Engine::GetInput().GetButtonPressed(WPAD_BUTTON_HOME))
		Engine::SetRootEntity<MainMenuRoot>();
}

void PracticeRoot::OnUnload() {
	Engine::GetGUI().ShowCursors(true);
}

PracticeRoot::SceneData& PracticeRoot::GetData()
{
	return *((SceneData*)m_userData);
}

const PracticeRoot::SceneData& PracticeRoot::GetData() const
{
	return *((SceneData*)m_userData);
}