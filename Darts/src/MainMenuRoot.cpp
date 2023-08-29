#include "MainMenuRoot.h"
#include "DartEntity.h"
#include "MeshEntity.h"
#include "engine/CameraEntity.h"
#include "engine/LightEntity.h"
#include "engine/GUI.h"
#include "DartEntity.h"
#include "PracticeRoot.h"
#include "engine/Engine.h"
#include <new>

struct MainMenuRoot::SceneData {

};

MainMenuRoot::MainMenuRoot() {
	WD_STATIC_ASSERT(sizeof(MainMenuRoot::SceneData) <= sizeof(m_userData), "MainMenuRoot::SceneData too big");
	new(m_userData) SceneData();
}

MainMenuRoot::~MainMenuRoot() {
	GetData().~SceneData();
}

void MainMenuRoot::OnLoad() {
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
		roomLightTransform.SetPosition({ 0,0,5 });
		roomLightTransform.LookAt({ 0,0,0 }, { 0,1,0 });
		roomLight->SetColor({ 0xff, 0xff, 0xff, 0xff });
		roomLight->SetSpecularEnabled(false);
		roomLight->SetProperties(8, 30, 0.99);
	}

	// Setup room models
	AddChild<MeshEntity>("./assets/dense_plane.mesh", "./assets/room.img");
	AddChild<MeshEntity>("./assets/board.mesh", "./assets/board.img");
	Transform& cameraTransform = AddChild<CameraEntity>()->GetTransform();
	cameraTransform.SetPosition({ 0,0,1.5f });
	cameraTransform.LookAt({ 0,0,0 }, { 0,1,0 });
}

void MainMenuRoot::OnRender() {
	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize;
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::Begin("Main Menu", nullptr, flags);
	ImGui::Text("Darts!");
	if (ImGui::Button("Practice")) {
		Engine::SetRootEntity<PracticeRoot>();
	}

	ImGui::BeginDisabled();
	ImGui::Button("Play");
	ImGui::EndDisabled();
	if (ImGui::Button("Quit")) Engine::Quit();
	ImGui::End();
}

MainMenuRoot::SceneData& MainMenuRoot::GetData()
{
	return *((SceneData*)m_userData);
}

const MainMenuRoot::SceneData& MainMenuRoot::GetData() const
{
	return *((SceneData*)m_userData);
}
