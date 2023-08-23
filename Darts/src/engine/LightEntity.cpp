#include "engine/LightEntity.h"
#include "engine/Engine.h"
#include "engine/Renderer.h"

LightEntity::LightEntity() {
	m_diffuseIndex = 0;
	m_specularIndex = 0;
	SetSpecularEnabled(false);
	SetColor({ 0xFF, 0xFF, 0xFF, 0xFF });
	SetCutoff(45);
	//GetTransform().Reset();
	Engine::Log("Light Create");
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
		Renderer& renderer = Engine::GetRenderer();
		if (m_specularEnabled) {
			m_specularIndex = renderer.AcquireSpecularLightIndex();
			if (!m_specularIndex && m_diffuseIndex) {
				renderer.ReleaseDiffuseLightIndex(m_diffuseIndex);
				Disable();
				return;
			}

			GX_LoadLightObj(&m_specular, m_specularIndex);
		}
		else {
			if (m_specularIndex) renderer.ReleaseSpecularLightIndex(m_specularIndex);
		}
	}
}

void LightEntity::SetCutoff(float cutoff)
{
	//GX_InitLightSpot(&m_diffuse, cutoff, GX_SP_COS);
}

void LightEntity::OnUpdate()
{
	Renderer& renderer = Engine::GetRenderer();
	guVector pos = { 0,0,0 };
	guVector dir = { 0, 0, -1 };
	guVecMultiply(const_cast<Mtx&>(renderer.GetActiveCameraTransform().GetMatrix()), &pos, &pos);
	GX_InitLightPos(&m_diffuse, pos.x, pos.y, pos.z);
	GX_InitLightColor(&m_diffuse, { 0xFF, 0xFF, 0xFF, 0xFF });
	GX_InitLightDir(&m_diffuse, dir.x, dir.y, dir.z); // doesn't really matter what direction		
	GX_LoadLightObj(&m_diffuse, GX_LIGHT0);


	//GX_InitLightPos(&m_diffuse, 20, 20, 30);
	//GX_InitLightColor(&m_diffuse, (GXColor) { 192, 192, 192, 0xFF });
	//// Initialise "k" parameters.
	//GX_InitLightDistAttn(&m_diffuse, 1.0, 1.0, GX_DA_OFF);
	//// Initialise "a" parameters.
	//GX_InitLightSpot(&m_diffuse, 0.0, GX_SP_OFF);
	//GX_InitLightDir(&m_diffuse, 0.0, 0.0, 0.0);
	//GX_LoadLightObj(&m_diffuse, m_diffuseIndex);


	//Renderer& renderer = Engine::GetRenderer();
	//guVector pos = { 0,0,8 };
	//guVector dir = { 0, 0, -1 };
	//guVecMultiply(const_cast<Mtx&>(renderer.GetActiveCameraTransform().GetMatrix()), &pos, &pos);
	////guVecMultiply(const_cast<Mtx&>(renderer.GetActiveCameraTransform().GetMatrix()), &dir, &dir);
	//GX_InitLightPos(&m_diffuse, pos.x, pos.y, pos.z);
	//GX_InitLightDir(&m_diffuse, dir.x, dir.y, dir.z);
	////GX_InitLightAttn(&m_diffuse, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	//GX_InitLightDistAttn(&m_diffuse, 1000.0f, 0.9f, GX_DA_GENTLE);
	//GX_InitLightSpot(&m_diffuse, 45, GX_SP_OFF);


	//Renderer& renderer = Engine::GetRenderer();
	//Transform worldT;
	//GetWorldTransform(worldT);
	//guVector tmp = { 0,0,0 };
	////guVecMultiply(const_cast<Mtx&>(renderer.GetActiveCameraTransform().GetMatrix()), &tmp, &tmp);
	////GX_InitLightPos(&m_diffuse, tmp.x, tmp.y, tmp.z);
	//tmp = { 0, 0, 1 };
	//GX_InitSpecularDir(&m_specular, tmp.x, tmp.y, tmp.z);
	////GX_InitLightDir(&m_diffuse, tmp.x, tmp.y, tmp.z);
	////if (GetEnabled()) {
	////	GX_LoadLightObj(&m_diffuse, m_diffuseIndex);
	////	if (m_specularEnabled) {
	////		GX_LoadLightObj(&m_specular, m_specularIndex);
	////	}

	////	Engine::Log("Light Update");
	////}
}

void LightEntity::OnEnable()
{
	Renderer& renderer = Engine::GetRenderer();
	m_diffuseIndex = renderer.AcquireDiffuseLightIndex();
	if (!m_diffuseIndex) {
		Disable();
		return;
	}

	if (m_specularEnabled) {
		m_specularIndex = renderer.AcquireSpecularLightIndex();
		if (!m_specularIndex) {
			renderer.ReleaseDiffuseLightIndex(m_diffuseIndex);
			Disable();
			return;
		}

		GX_LoadLightObj(&m_specular, m_specularIndex);
	}

	//GX_LoadLightObj(&m_diffuse, m_diffuseIndex);
	Engine::Log("Light Enabled");
}

void LightEntity::OnDisable()
{
	Renderer& renderer = Engine::GetRenderer();
	if (m_diffuseIndex) renderer.ReleaseDiffuseLightIndex(m_diffuseIndex);
	if (m_specularIndex) renderer.ReleaseSpecularLightIndex(m_specularIndex);
	Engine::Log("Light Disabled");
}
