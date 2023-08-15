#include "Entity.h"
#include "Engine.h"

Entity::Entity(bool enabled) {
	m_parent = nullptr;
	m_childPtr = nullptr;
	m_nextChild = nullptr;
	m_childCount = 0;
	m_enabled = enabled;
}

Entity::~Entity() {

}

WD_NODISCARD const Transform& Entity::GetTransform() const
{
	return m_transform;
}

WD_NODISCARD bool Entity::HasIdentityTransform() const
{
	return m_identityTransform;
}

bool Entity::GetEnabled() const
{
	return m_enabled;
}

Entity* Entity::GetParent() const
{
	return m_parent;
}

WD_NODISCARD Entity* Entity::GetChild(size_t index) const
{
	if (index >= m_childCount) return nullptr;
	Entity* val = m_childPtr;
	for (size_t i = 0; i < index; ++i)
		val = val->m_nextChild;
	return val;
}

uint16_t Entity::GetChildCount() const
{
	return m_childCount;
}

void* Entity::AllocateChildMem(size_t size)
{
	return Engine::AllocateSceneMem(size);
}

void Entity::Update()
{
	if (!m_enabled || !m_loaded) return;
	OnUpdate();
	Entity* itr = m_childPtr;
	while (itr)
	{
		m_childPtr->Update();
		itr = itr->m_nextChild;
	}
}

void Entity::Render()
{
	if (!m_enabled || !m_loaded) return;
	if (m_identityTransform) {
		Engine::GetRenderer().PushIdentityTransform();
	}
	else {
		Engine::GetRenderer().PushTransform(m_transform);
	}

	OnRender();
	Entity* itr = m_childPtr;
	while (itr)
	{
		m_childPtr->Render();
		itr = itr->m_nextChild;
	}
}

void Entity::Load()
{
	OnLoad();
	m_loaded = true;

	Entity* itr = m_childPtr;
	while (itr)
	{
		m_childPtr->Load();
		itr = itr->m_nextChild;
	}

	if (m_enabled)
		OnEnable();
}

void Entity::Unload()
{
	if (m_enabled)
		OnDisable();

	Entity* itr = m_childPtr;
	while (itr)
	{
		m_childPtr->Unload();
		itr = itr->m_nextChild;
	}

	OnUnload();
	m_loaded = false;
}

void Entity::AddChild(Entity* child)
{
	if (m_childCount == 0) {
		m_childPtr = child;
	}
	else {
		GetChild(m_childCount - 1)->m_nextChild = child;
	}

	++m_childCount;
}

void Entity::UpdateTransform(const Transform& t)
{
	m_transform = t;
	MarkTransformDirty();
}

void Entity::ResetTransform()
{
	m_transform.Reset();
	m_identityTransform = true;
}

void Entity::Disable()
{
	if (!m_enabled) return;
	m_enabled = false;
	if (m_loaded) OnDisable();
}

void Entity::Enable()
{
	if (m_enabled) return;
	m_enabled = true;
	if (m_loaded) OnEnable();
}

void Entity::MarkTransformDirty()
{
	m_identityTransform = false;
}

WD_NODISCARD Transform& Entity::GetTransform()
{
	return m_transform;
}
