#include "Entity.h"

#include <stdint.h>
#include <new>

#include "Transform.h"
#include "Logger.h"
#include "Renderer.h"

namespace wiidarts {
	Entity::Entity() {
		_childCount = 0;
		_children = nullptr;
		_parent = nullptr;
	}

	Entity::~Entity() {
		clearChildren();
		if (_parent) _parent->removeChild(*this);
	}

	const Transform& Entity::getTransform() const {
		return _transform;
	}

	Transform& Entity::getTransform()
	{
		return _transform;
	}

	void Entity::update() {
		Renderer::getInstance().pushTransform(_transform);
		onUpdate();
		EntityNode* node = _children;
		while (node)
		{
			node->Value.update();
			node = node->Next;
		}

		Renderer::getInstance().popTransform();
	}

	void Entity::onUpdate() {

	}

	int32_t Entity::addChild(Entity& child, uint16_t index)
	{
		if (child._parent) child._parent->removeChild(child);
		if (index == 0 || _children == nullptr) {
			EntityNode* oldNode = _children;
			_children = new(std::nothrow) EntityNode{ oldNode, child };
			if (_children == nullptr) {
				_children = oldNode;
				Logger::error("Entity::addChild failed to allocate EntityNode, Child Entity not added");
				return -1;
			}

			child.grab();
			child._parent = this;
			++_childCount;
			return 0;
		}
		else {
			uint16_t currentIndex = 1;
			EntityNode* currentNode = _children;
			while (currentIndex != index && currentNode->Next) {
				currentNode = currentNode->Next;
				++currentIndex;
			}

			EntityNode* oldNode = currentNode->Next;
			currentNode->Next = new(std::nothrow) EntityNode{ oldNode, child };
			if (currentNode->Next == nullptr) {
				currentNode->Next = oldNode;
				Logger::error("Entity::addChild failed to allocate EntityNode, Child Entity not added");
				return -1;
			}

			child.grab();
			child._parent = this;
			++_childCount;
			return (int32_t)currentIndex;
		}
	}

	bool Entity::removeChild(Entity& child)
	{
		EntityNode* lastNode = nullptr;
		EntityNode* currentNode = _children;
		while (currentNode && &(currentNode->Value) != &child)
		{
			lastNode = currentNode;
			currentNode = currentNode->Next;
		}

		if (currentNode == nullptr) return false;
		child._parent = nullptr;
		child.drop();
		if (lastNode == nullptr) {
			_children = currentNode->Next;
		}
		else {
			lastNode->Next = currentNode->Next;
		}

		delete currentNode;
		return true;
	}

	bool Entity::removeChild(uint16_t index)
	{
		if (index >= _childCount) return false;
		uint16_t currentIndex = 0;
		EntityNode* lastNode = nullptr;
		EntityNode* currentNode = _children;
		while (currentIndex != index)
		{
			lastNode = currentNode;
			currentNode = currentNode->Next;
			++currentIndex;
		}

		currentNode->Value._parent = nullptr;
		currentNode->Value.drop();
		if (lastNode == nullptr) {
			_children = currentNode->Next;
		}
		else {
			lastNode->Next = currentNode->Next;
		}

		delete currentNode;
		return true;
	}

	void Entity::clearChildren()
	{
		_childCount = 0;
		EntityNode* topNode = _children;
		_children = nullptr;
		while (topNode)
		{
			EntityNode* currentNode = topNode;
			topNode->Value._parent = nullptr;
			topNode->Value.drop();
			topNode = topNode->Next;
			delete currentNode;
		}
	}

	Entity* Entity::getChild(uint16_t index) const
	{
		uint16_t currentIndex = 0;
		EntityNode* currentNode = _children;
		while (currentIndex != index && currentNode)
		{
			currentNode = currentNode->Next;
			++currentIndex;
		}

		if (currentNode) return &(currentNode->Value);
		return nullptr;
	}

	uint16_t Entity::getChildCount() const {
		return _childCount;
	}

	Entity* Entity::getParent() const {
		return _parent;
	}
}