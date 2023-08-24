#include "engine/LightEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include "engine/AssetLoader.h"
#include <cstring>

LightEntity::LightEntity() {
	m_diffuseIndex = 0;
	m_specularIndex = 0;
	m_mesh = nullptr;
	m_specularEnabled = false;
	SetColor({ 0xFF, 0x00, 0xFF, 0xFF });
	SetProperties(90, 10, 0.99f);
	SetSpecularShininess(50);
	for (int i = 0; i < 6; ++i)
		m_set[i] = 0;
	m_set[6] = 90;
	m_set[7] = 10;
	m_set[8] = 0.99;
}

LightEntity::~LightEntity() {

}

void LightEntity::OnLoad() {
	m_mesh = AssetLoader::LoadMesh("./assets/cube.mesh");
}

void LightEntity::OnRender() {
	m_mesh->Render();
	ImGui::Begin("Light");
	ImGui::SliderAngle("Rot X", m_set, -180, 180);
	ImGui::SliderAngle("Rot Y", m_set + 1, -180, 180);
	ImGui::SliderAngle("Rot Z", m_set + 2, -180, 180);
	ImGui::SliderFloat("Pos X", m_set + 3, -10, 10);
	ImGui::SliderFloat("Pos Y", m_set + 4, -10, 10);
	ImGui::SliderFloat("Pos Z", m_set + 5, -10, 10);
	ImGui::SliderFloat("Radius", m_set + 6, 1, 90);
	ImGui::SliderFloat("Dist", m_set + 7, 0, 100);
	ImGui::SliderFloat("intensity", m_set + 8, 0, 1);
	ImGui::End();
	SetProperties(m_set[6], m_set[7], m_set[8]);
}

void LightEntity::SetColor(const GXColor& color)
{
	GX_InitLightColor(&m_diffuse, color);
	GX_InitLightColor(&m_specular, color);
}

void LightEntity::SetSpecularEnabled(bool enabled)
{
	m_specularEnabled = enabled;
	if (GetEnabled()) {
		// Cycle the light to add/remove the specular light 
		Disable();
		Enable();
	}
}

void LightEntity::SetProperties(float radiusDeg, float dist, float intensity)
{
	GX_InitLightDistAttn(&m_diffuse, dist, intensity, GX_DA_GENTLE);
	GX_InitLightSpot(&m_diffuse, radiusDeg, GX_SP_COS);

}

void LightEntity::SetSpecularShininess(uint8_t shine)
{
	GX_InitLightShininess(&m_specular, (float)shine);
}

void LightEntity::UpdateLight()
{
	Renderer& renderer = Engine::GetRenderer();
	guVector pos = GetTransform().GetPosition();
	guVector dir = GetTransform().GetForward();
	guVecMultiply(const_cast<Mtx&>(renderer.GetActiveCameraTransform().GetMatrix()), &pos, &pos);
	GX_InitLightPos(&m_diffuse, pos.x, pos.y, pos.z);
	GX_InitLightDir(&m_diffuse, dir.x, dir.y, dir.z);
	guVecMultiplySR(const_cast<Mtx&>(renderer.GetActiveCameraNormalTransform().GetMatrix()), &dir, &dir);
	GX_InitSpecularDir(&m_specular, dir.x, dir.y, dir.z);
	GX_LoadLightObj(&m_diffuse, m_diffuseIndex);
	if (m_specularIndex) GX_LoadLightObj(&m_specular, m_specularIndex);
}

void LightEntity::OnUpdate()
{
	Transform& t = GetTransform();
	MarkTransformDirty();
	t.Reset();
	t.Translate({ m_set[3], m_set[4], m_set[5] });
	t.Rotate({ m_set[0], m_set[1], m_set[2] });
	t.Scale({ 0.01f, 0.01f, 0.01f });
}

void LightEntity::OnEnable()
{
	Renderer& renderer = Engine::GetRenderer();
	m_diffuseIndex = renderer.AcquireDiffuseLightIndex(this);
	if (!m_diffuseIndex) {
		Engine::Log("Failed to enable light (Diffuse)");
		Disable();
		return;
	}

	if (m_specularEnabled) {
		m_specularIndex = renderer.AcquireSpecularLightIndex(this);
		if (!m_specularIndex) {
			Engine::Log("Failed to enable light (Specular)");
			renderer.ReleaseDiffuseLightIndex(m_diffuseIndex);
			Disable();
			return;
		}
	}
}

void LightEntity::OnDisable()
{
	Renderer& renderer = Engine::GetRenderer();
	if (m_diffuseIndex) renderer.ReleaseDiffuseLightIndex(m_diffuseIndex);
	if (m_specularIndex) renderer.ReleaseSpecularLightIndex(m_specularIndex);
}
