#include "GUICameraEntity.h"
#include "engine/GUI.h"

GUICameraEntity::GUICameraEntity() {
	for (int i = 0; i < 6; ++i)
		m_set[i] = 0;
	m_set[5] = 2.37;
}

GUICameraEntity::~GUICameraEntity() {

}

void GUICameraEntity::OnUpdate() {
	Transform& t = GetTransform();
	t.Reset();
	t.Rotate({ m_set[0], m_set[1], m_set[2] });
	t.Translate((guVector) { m_set[3], m_set[4], m_set[5] });
	Super::OnUpdate();
}

void GUICameraEntity::OnRender() {
	ImGui::Begin("Camera");
	ImGui::SliderAngle("Rot X", m_set, -180, 180);
	ImGui::SliderAngle("Rot Y", m_set + 1, -180, 180);
	ImGui::SliderAngle("Rot Z", m_set + 2, -180, 180);
	ImGui::SliderFloat("Pos X", m_set + 3, -10, 10);
	ImGui::SliderFloat("Pos Y", m_set + 4, -10, 10);
	ImGui::SliderFloat("Pos Z", m_set + 5, -10, 10);
	ImGui::End();
}
