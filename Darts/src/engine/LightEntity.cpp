#include "engine/LightEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"
#include <cstring>

LightEntity::LightEntity() {
	memset(&m_diffuse, 0, sizeof(m_diffuse));
	memset(&m_specular, 0, sizeof(m_specular));
	m_diffuseIndex = 0;
	m_specularIndex = 0;
	//m_mesh = nullptr;
	m_specularEnabled = false;
	SetColor({ 0xFF, 0xFF, 0xFF, 0xFF });
	SetProperties(90, 10, 0.99f);
	SetSpecularShininess(50);
}

LightEntity::~LightEntity() {

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
	guVector normDir = GetTransform().GetForward();
	guVecScale(&normDir, &normDir, -1);
	guVecNormalize(&normDir);
	guVector dir = normDir;
	guVecMultiply(const_cast<Mtx&>(renderer.GetActiveCameraTransform().GetMatrix()), &pos, &pos);
	guVecMultiplySR(const_cast<Mtx&>(renderer.GetActiveCameraTransform().GetMatrix()), &dir, &dir);
	GX_InitLightPos(&m_diffuse, pos.x, pos.y, pos.z);
	GX_InitLightDir(&m_diffuse, dir.x, dir.y, dir.z);
	GX_LoadLightObj(&m_diffuse, m_diffuseIndex);
	if (m_specularEnabled) {
		dir = normDir;
		guVecMultiplySR(const_cast<Mtx&>(renderer.GetActiveCameraNormalTransform().GetMatrix()), &dir, &dir);
		GX_InitSpecularDir(&m_specular, dir.x, dir.y, dir.z);
		GX_LoadLightObj(&m_specular, m_specularIndex);
	}
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
