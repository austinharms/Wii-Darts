#ifndef DARTS_LIGHT_ENTITY_H_
#define DARTS_LIGHT_ENTITY_H_
#include "Core.h"
#include "Entity.h"
#include <ogc/gx.h>

class LightEntity : public Entity
{
public:
	LightEntity();
	virtual ~LightEntity();
	void SetColor(const GXColor& color);
	void SetSpecularEnabled(bool enabled);
	void SetCutoff(float cutoff);
	using Entity::GetTransform;
	using Entity::MarkTransformDirty;

protected:
	void OnUpdate() WD_OVERRIDE;
	void OnEnable() WD_OVERRIDE;
	void OnDisable() WD_OVERRIDE;

private:
	GXLightObj m_diffuse;
	GXLightObj m_specular;
	uint8_t m_diffuseIndex;
	uint8_t m_specularIndex;
	bool m_specularEnabled;
};
#endif // !DARTS_LIGHT_ENTITY_H_