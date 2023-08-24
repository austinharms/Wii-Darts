#ifndef DARTS_LIGHT_ENTITY_H_
#define DARTS_LIGHT_ENTITY_H_
#include "Core.h"
#include "Entity.h"
#include <ogc/gx.h>

class Renderer;

class LightEntity : public Entity
{
public:
	LightEntity();
	virtual ~LightEntity();
	void SetColor(const GXColor& color);
	void SetSpecularEnabled(bool enabled);
	void SetProperties(float radiusDeg, float dist, float intensity);
	// Value must be between 4 and 255
	void SetSpecularShininess(uint8_t shine);
	using Entity::GetTransform;
	using Entity::MarkTransformDirty;

protected:
	void OnEnable() WD_OVERRIDE;
	void OnDisable() WD_OVERRIDE;

private:
	friend class Renderer;

	GXLightObj m_diffuse;
	GXLightObj m_specular;
	uint8_t m_diffuseIndex;
	uint8_t m_specularIndex;
	bool m_specularEnabled;

	void UpdateLight();
};
#endif // !DARTS_LIGHT_ENTITY_H_