#include "PracticeRoot.h"
#include "engine/Engine.h"
#include "engine/Input.h"
#include "engine/LightEntity.h"
#include "engine/CameraEntity.h"
#include "engine/moreFMath.h"
#include "MeshEntity.h"
#include "DartEntity.h"
#include "DartThrowUI.h"
#include "MainMenuRoot.h"
#include <new>

struct PracticeRoot::SceneData {
	DartEntity* darts[3];
	DartThrowUI* throwUI;
	CameraEntity* camera;
	uint8_t activeDart;
	bool camFollowDart;

	SceneData() {
		darts[0] = nullptr;
		darts[1] = nullptr;
		darts[2] = nullptr;
		throwUI = nullptr;
		camera = nullptr;
		activeDart = 2;
		camFollowDart = false;
	}
};

// TODO Fix input mapping to use appropriate values
void mapNormalThrowVector(const guVector& accel, guVector& outVector) {
	outVector.x = map(clamp(accel.x, -2, 2), -2, 2, -1, 1);
	outVector.y = map(clamp(accel.y, 1, 5), 1, 5, -1, 1);
	outVector.z = ((1 - (outVector.x * outVector.x)) + (outVector.y * outVector.y)) - 1;
}

PracticeRoot::PracticeRoot() {
	WD_STATIC_ASSERT(sizeof(PracticeRoot::SceneData) <= sizeof(m_userData), "PracticeRoot::SceneData too big");
	new(m_userData) SceneData();
}

PracticeRoot::~PracticeRoot() {
	GetData().~SceneData();
}

void PracticeRoot::OnLoad() {
	SceneData& data = GetData();
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

	// Setup main camera
	data.camera = AddChild<CameraEntity>();

	// Setup Meshes 
	AddChild<MeshEntity>("./assets/dense_plane.mesh", "./assets/room.img");
	AddChild<MeshEntity>("./assets/board.mesh", "./assets/board.img");
	for (int i = 0; i < 3; ++i) {
		data.darts[i] = AddChild<DartEntity>();
		//data.darts[i]->Disable();
	}

	// Add this at the end so the dart cam overrides all previous cameras
	data.throwUI = AddChild<DartThrowUI>();
	//data.throwUI->Disable();
}

void PracticeRoot::OnUpdate() {
	SceneData& data = GetData();
	Input& input = Engine::GetInput();

	guVector accel;
	guVector ori;
	input.GetAccel(accel);
	input.GetOrientation(ori);

	Transform& dartTransform = data.darts[data.activeDart]->GetTransform();
	if (input.GetButtonPressed(WPAD_BUTTON_B)) {
		if (++data.activeDart >= 3) data.activeDart = 0;
		DartEntity* dart = data.darts[data.activeDart];
		dart->Enable();
		data.camFollowDart = false;
		dartTransform = dart->GetTransform();
		dartTransform.Reset();
		dartTransform.SetPosition({ 0,0,2.37f });
		dartTransform.LookAt({ 0, 0, 0 }, { 0, 1, 0 });
	}
	else if (input.GetButtonDown(WPAD_BUTTON_B)) {
		guVector nVec;
		mapNormalThrowVector(accel, nVec);
		data.throwUI->SetThrowPower(nVec.z, nVec.x);
	}
	else if (input.GetButtonReleased(WPAD_BUTTON_B)) {
		DartEntity* dart = data.darts[data.activeDart];
		guVector nVec;
		mapNormalThrowVector(accel, nVec);
		data.throwUI->SetThrowPower(nVec.z, nVec.x);
		dart->Throw({ nVec.x, nVec.y * 3, -(10 + (nVec.z * 5)) });
		data.camFollowDart = true;
	}

	Transform& camTransform = data.camera->GetTransform();
	if (data.camFollowDart) {
		camTransform.SetPosition({ 2,0,3 });
		camTransform.LookAt({ 0,0,-1 }, { 0,1,0 });
		camTransform.LookAt(dartTransform.GetPosition(), { 0,1,0 });
		data.camera->SetFOV(map(clamp(dartTransform.GetPosition().z, 0, 2), 0, 2, 8, 60));
	}
	else {
		camTransform.SetPosition({ 0,0,2.37f });
		camTransform.LookAt({ 0,0,0 }, { 0,1,0 });
		data.camera->SetFOV(60);
	}

	if (Engine::GetInput().GetButtonPressed(WPAD_BUTTON_HOME)) {
		Engine::SetRootEntity<MainMenuRoot>();
		Engine::Quit();
		return;
	}
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