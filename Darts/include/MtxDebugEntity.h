#ifndef DARTS_MTX_MESH_ENTITY_H_
#define DARTS_MTX_MESH_ENTITY_H_
#include "MeshEntity.h"
#include "engine/GUI.h"

#ifndef WD_DEBUG_LOOKAT
#define WD_DEBUG_LOOKAT 0
#endif // !WD_DEBUG_LOOKAT

#ifndef WD_DEBUG_TRANSFORM
#define WD_DEBUG_TRANSFORM 0
#endif // !WD_DEBUG_TRANSFORM

#if WD_DEBUG_LOOKAT && WD_DEBUG_TRANSFORM
#error Can only define WD_DEBUG_LOOKAT or WD_DEBUG_TRANSFORM not both
#endif

template<class EntityT>
class MtxDebugEntity : public EntityT
{
public:
	template <typename ...Args>
	MtxDebugEntity(Args&&... args) : EntityT(std::forward<Args>(args)...), m_transform(EntityT::GetTransform()) {
#if WD_DEBUG_LOOKAT || WD_DEBUG_TRANSFORM
		for (int i = 0; i < 6; ++i) m_set[i] = 0;
#endif
#if WD_DEBUG_LOOKAT
		m_set[1] = 1;
		m_set[5] = -1;
#endif
	}

	virtual ~MtxDebugEntity() = default;

protected:
	void OnRender() WD_OVERRIDE {
		ImGui::Begin("Debug Mtx");
		ImGui::DragFloat4("#1", t.GetMatrix()[0], 0.01f);
		ImGui::DragFloat4("#2", t.GetMatrix()[1], 0.01f);
		ImGui::DragFloat4("#3", t.GetMatrix()[2], 0.01f);
		ImGui::End();

#if WD_DEBUG_TRANSFORM
		ImGui::Begin("Debug Transform");
		ImGui::DragFloat3("Rot", m_set, 0.01f);
		ImGui::DragFloat3("Pos", m_set + 3, 0.01f);
		ImGui::End();
		m_transform.Reset();
		m_transform.Translate({ m_set[3], m_set[4], m_set[5] });
		m_transform.Rotate({ m_set[0], m_set[1], m_set[2] });
#endif
#if WD_DEBUG_LOOKAT
		ImGui::Begin("Debug LookAt");
		ImGui::DragFloat3("Up Dir", m_set, 0.01f);
		ImGui::DragFloat3("Look Pos", m_set + 3, 0.01f);
		ImGui::End();
		m_transform.LookAt({ m_set[3], m_set[4], m_set[5] }, { m_set[0], m_set[1], m_set[2] });
#endif

		EntityT::OnRender();
	}

private:
	Transform& m_transform;
#if WD_DEBUG_LOOKAT || WD_DEBUG_TRANSFORM
	float m_set[6];
#endif
};
#endif // !DARTS_MTX_MESH_ENTITY_H_