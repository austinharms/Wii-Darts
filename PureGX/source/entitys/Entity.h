#ifndef ENTITY_H_
#define ENTITY_H_
#include "../Vector3f.h"

class Entity
{
public:
	Entity(Vector3f pos, Vector3f rot, Vector3f scale) : localPos(pos), localRot(rot), localScale(scale), _parent(nullptr), _children(nullptr) { }
	Entity() : localPos(0), localRot(0), localScale(1), _parent(nullptr), _children(nullptr) { }
	void update() {
		onUpdate();
		EntityNode* curNode = _children;
		while (curNode != nullptr)
		{
			curNode->value->update();
			curNode = curNode->next;
		}
	}

	Vector3f getLocalPosition() const {
		return localPos;
	}

	Vector3f getLocalRotation() const {
		return localRot;
	}

	Vector3f getLocalScale() const {
		return localScale;
	}

	Vector3f getWorldPosition() const {
		Vector3f pos = localPos;
		Entity* p = _parent;
		while (p != nullptr)
		{
			pos += (p->localPos * p->localScale);
			p = p->_parent;
		}

		return pos;
	}

	Vector3f getWorldRotation() const {
		Vector3f rot = localRot;
		Entity* p = _parent;
		while (p != nullptr)
		{
			rot += p->localRot;
			p = p->_parent;
		}

		return rot;
	}

	Vector3f getWorldScale() const {
		Vector3f scale = localScale;
		Entity* p = _parent;
		while (p != nullptr)
		{
			scale *= p->localScale;
			p = p->_parent;
		}

		return scale;
	}

	void addChild(Entity* child) {
		child->_parent = this;
		_children = new EntityNode(child, _children);
	}

	Entity* removeChild(Entity* child) {
		EntityNode* lastNode = nullptr;
		EntityNode* curNode = _children;
		while (curNode != nullptr)
		{
			if (curNode->value == child) {
				if (lastNode == nullptr) {
					_children = curNode->next;
				}
				else 
				{
					lastNode->next = curNode->next;
				}

				delete curNode;
				return child;
			}

			lastNode = curNode;
			curNode = curNode->next;
		}

		return nullptr;
	}

	Entity* getParent() const { return _parent; }

	uint32_t getChildCount() const { 
		uint32_t count = 0;
		EntityNode* curNode = _children;
		while (curNode != nullptr)
		{
			++count;
			curNode = curNode->next;
		}

		return count;
	}

	Entity* getChild(uint32_t index) {
		uint32_t count = 0;
		EntityNode* curNode = _children;
		while (curNode != nullptr)
		{
			if (count++ == index)
				return curNode->value;
			curNode = curNode->next;
		}

		return nullptr;
	}

protected:
	Vector3f localPos;
	Vector3f localRot;
	Vector3f localScale;
	virtual void onUpdate() {};

private:
	struct EntityNode
	{
		EntityNode(Entity* e, EntityNode* n) {
			value = e;
			next = n;
		}

		Entity* value;
		EntityNode* next;
	};

	Entity* _parent = nullptr;
	EntityNode* _children = nullptr;
};
#endif // !ENTITY_H_
