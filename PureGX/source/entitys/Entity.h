#ifndef ENTITY_H_
#define ENTITY_H_
#include "../Vector3f.h"

class Entity
{
public:
	Entity(Vector3f pos) : localPos(pos), _parent(nullptr), _children(nullptr) { }
	Entity() : localPos(0), _parent(nullptr), _children(nullptr) { }
	void update() {
		onUpdate();
		EntityNode* curNode = _children;
		while (curNode != nullptr)
		{
			curNode->value->update();
			curNode->next;
		}
	}

	Vector3f getLocalPosition() const {
		return localPos;
	}

	Vector3f getWorldPosition() const {
		Vector3f pos = localPos;
		Entity* p = _parent;
		while (p != nullptr)
		{
			pos = pos + p->localPos;
			p = p->_parent;
		}

		return pos;
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
			curNode->next;
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
			curNode->next;
		}

		return nullptr;
	}

protected:
	Vector3f localPos;
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
