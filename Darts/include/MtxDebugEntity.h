#ifndef DARTS_MTX_MESH_ENTITY_H_
#define DARTS_MTX_MESH_ENTITY_H_
#include "MeshEntity.h"
#include "engine/GUI.h"

template<class EntityT>
class MtxDebugEntity : public EntityT
{
public:
	template <typename ...Args>
	MtxDebugEntity(Args&&... args) : EntityT(std::forward<Args>(args)...), t(EntityT::GetTransform()) {
		for (int i = 0; i < 6; ++i) m_set[i] = 0;
		m_set[1] = 1;
		m_set[5] = -1;
	}

	virtual ~MtxDebugEntity() = default;

protected:
	void OnRender() WD_OVERRIDE {
		ImGui::Begin("Debug Mtx");
		ImGui::DragFloat4("#1", t.GetMatrix()[0], 0.01f);
		ImGui::DragFloat4("#2", t.GetMatrix()[1], 0.01f);
		ImGui::DragFloat4("#3", t.GetMatrix()[2], 0.01f);
		ImGui::End();
		//ImGui::Begin("Mesh Set");
		//ImGui::DragFloat3("Rot", m_set, 0.01f);
		//ImGui::DragFloat3("Pos", m_set + 3, 0.01f);
		//ImGui::End();
		//t.Reset();
		//t.Translate({ m_set[3], m_set[4], m_set[5] });
		//t.Rotate({ m_set[0], m_set[1], m_set[2] });

		ImGui::Begin("Debug Set");
		ImGui::DragFloat3("Up", m_set, 0.01f);
		ImGui::DragFloat3("Look Pos", m_set + 3, 0.01f);
		ImGui::End();
		//t.Reset();
		t.LookAt({ m_set[3], m_set[4], m_set[5] }, { m_set[0], m_set[1], m_set[2] });
		EntityT::OnRender();
	}

private:
	Transform& t;
	float m_set[6];
};
#endif // !DARTS_MTX_MESH_ENTITY_H_