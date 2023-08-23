#include "CameraEntity.h"
#include "engine/GUI.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

CameraEntity::CameraEntity() {
	for (int i = 0; i < 6; ++i)
		m_set[i] = 0;
	m_set[5] = -0.2;
}

CameraEntity::~CameraEntity() {}

void CameraEntity::OnUpdate() {
	Transform& t = GetTransform();
	t.Reset();
	t.Translate((guVector) { m_set[3], m_set[4], m_set[5] });
	Mtx temp;
	if (m_set[0]) {
		guVector axis{ 1, 0, 0 };
		guMtxRotAxisRad(temp, &axis, m_set[0]);
		guMtxConcat(temp, t.GetMatrix(), t.GetMatrix());
	}

	if (m_set[1]) {
		guVector axis{ 0, 1, 0 };
		guMtxRotAxisRad(temp, &axis, m_set[1]);
		guMtxConcat(temp, t.GetMatrix(), t.GetMatrix());
	}

	if (m_set[2]) {
		guVector axis{ 0, 0, 1 };
		guMtxRotAxisRad(temp, &axis, m_set[2]);
		guMtxConcat(temp, t.GetMatrix(), t.GetMatrix());
	}

	Engine::GetRenderer().SetCameraTransform(t);
}

void CameraEntity::OnRender() {
	ImGui::Begin("Camera");
	ImGui::SliderAngle("Rot X", m_set, -180, 180);
	ImGui::SliderAngle("Rot Y", m_set + 1, -180, 180);
	ImGui::SliderAngle("Rot Z", m_set + 2, -180, 180);
	ImGui::SliderFloat("Pos X", m_set + 3, -10, 10);
	ImGui::SliderFloat("Pos Y", m_set + 4, -10, 10);
	ImGui::SliderFloat("Pos Z", m_set + 5, -10, 10);
	ImGui::End();
}