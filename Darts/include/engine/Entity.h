#ifndef DARTS_ENTITY_H_
#define DARTS_ENTITY_H_
#include "Core.h"
#include "Transform.h"
#include <ogc/gu.h>
#include <utility>
#include <new>

class RootEntity;

class Entity
{
public:
	Entity(bool enabled = true);
	virtual ~Entity();
	WD_NOCOPY(Entity);

	WD_NODISCARD const Transform& GetTransform() const;
	WD_NODISCARD bool GetEnabled() const;
	WD_NODISCARD Entity* GetParent() const;
	WD_NODISCARD Entity* GetChild(size_t index) const;
	WD_NODISCARD uint16_t GetChildCount() const;
	void GetWorldTransform(Transform& out) const;

protected:
	virtual void OnUpdate() {}
	virtual void OnRender() {}
	virtual void OnLoad() {}
	virtual void OnUnload() {}
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	void SetTransform(const Transform& t);
	void ResetTransform();
	void Disable();
	void Enable();
	WD_NODISCARD Transform& GetTransform();

	template <class EntityT, typename ...Args>
	EntityT* AddChild(Args&&... args) {
		void* childMem = AllocateChildMem(sizeof(EntityT));
		if (!childMem) return nullptr;
		EntityT* child = new(childMem) EntityT(std::forward<Args>(args)...);
		AddChild(static_cast<Entity*>(child));
		return child;
	}

private:
	friend class RootEntity;

	Transform m_transform;
	Entity* m_parent;
	Entity* m_childPtr;
	Entity* m_nextChild;
	uint16_t m_childCount;
	bool m_enabled;
	bool m_loaded;

	static void* AllocateChildMem(size_t size);

	void Update();
	void Render();
	void Load();
	void Unload();
	void AddChild(Entity* child);
};
#endif // !DARTS_ENTITY_H_
