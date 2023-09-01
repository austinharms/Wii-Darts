#include "DartEntity.h"
#include "engine/AssetLoader.h"
#include "engine/Engine.h"

DartEntity::DartEntity() {
	m_dartCoreMesh = nullptr;
	m_dartFinsMesh = nullptr;
	m_rot = 0;
	m_sim = false;
}

DartEntity::~DartEntity() {

}

void DartEntity::Throw(const guVector& velocity)
{
	m_velocity = velocity;
	m_rot = 0;
	m_sim = true;
}

void DartEntity::OnLoad() {
	m_dartCoreMesh = AssetLoader::LoadMesh("./assets/dart_core.mesh");
	m_dartFinsMesh = AssetLoader::LoadMesh("./assets/dart_fins.mesh");
	GetTransform().LookAt({ 0, 0, -10 }, { 1, 1, 0 });
}

void DartEntity::OnRender() {
	m_dartCoreMesh->Render(nullptr, 0xedededff);
	m_dartFinsMesh->Render(nullptr, 0x00eeffff);
}

void DartEntity::OnUpdate() {
	if (m_sim) {
		Transform& t = GetTransform();
		guVector pos = t.GetPosition();
		guVector newPos;
		m_velocity.y += 9.807f * Engine::GetDelta();
		guVecScale(&m_velocity, &newPos, Engine::GetDelta());
		guVecAdd(&newPos, &pos, &pos);
		guVecAdd(&pos, &newPos, &newPos);
		t.LookAt(newPos, { 0,1,0 });
		m_rot += -15 * Engine::GetDelta();
		t.Rotate(t.GetForward(), m_rot);
		if (pos.z <= 0.1f) {
			pos.z = 0.1f;
			m_sim = false;
		}

		if (pos.y < -5 || pos.y > 5 || pos.x > 5 || pos.x < -5) {
			m_sim = false;
		}

		t.SetPosition(pos);
	}
}