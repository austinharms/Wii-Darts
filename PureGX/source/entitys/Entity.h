#ifndef ENTITY_H_
#define ENTITY_H_
#include "../Vector3f.h"

class Entity
{
public:
	Entity(Vector3f pos, Vector3f rot, Vector3f scale) {
		_parent = nullptr;
		_children = nullptr;
		guMtxIdentity(_matrix);
		this->scale(scale);
		rotate(rot);
		translate(pos);
	}

	Entity() {
		_parent = nullptr;
		_children = nullptr;
		guMtxIdentity(_matrix);
	}

	void update() {
		onUpdate(_matrix);
		EntityNode* curNode = _children;
		while (curNode != nullptr)
		{
			curNode->value->update(_matrix);
			curNode = curNode->next;
		}
	}

	void update(Mtx& parentMatrix) {
		Mtx m;
		guMtxConcat(parentMatrix, _matrix, m);
		onUpdate(m);
		EntityNode* curNode = _children;
		while (curNode != nullptr)
		{
			curNode->value->update(m);
			curNode = curNode->next;
		}
	}

	Vector3f getPosition() const {
		return Vector3f(_matrix[0][3], _matrix[1][3], _matrix[2][3]);
	}

	void translate(const Vector3f& pos) {
		guMtxTransApply(_matrix, _matrix, pos.x, pos.y, pos.z);
	}

	void rotate(const Vector3f& rotation) {
		Vector3f pos = getPosition();
		setPosition(Vector3f(0));
		Mtx m;
		if (rotation.x != 0) {
			guMtxRotAxisDeg(m, &s_axixX, rotation.x);
			guMtxConcat(m, _matrix, _matrix);
		}

		if (rotation.y != 0) {
			guMtxRotAxisDeg(m, &s_axixY, rotation.y);
			guMtxConcat(m, _matrix, _matrix);
		}

		if (rotation.z != 0) {
			guMtxRotAxisDeg(m, &s_axixZ, rotation.z);
			guMtxConcat(m, _matrix, _matrix);
		}

		setPosition(pos);
	}

	void scale(const Vector3f& scale) {
		guMtxScaleApply(_matrix, _matrix, scale.x, scale.y, scale.z);
	}

	void setPosition(const Vector3f& pos) {
		_matrix[0][3] = pos.x;
		_matrix[1][3] = pos.y;
		_matrix[2][3] = pos.z;
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
	virtual void onUpdate(Mtx& renderMatrix) {};

private:
	static guVector s_axixX;
	static guVector s_axixY;
	static guVector s_axixZ;

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
	float _matrix[3][4];
};

guVector Entity::s_axixX = (guVector){ 1,0,0 };
guVector Entity::s_axixY = (guVector){ 0,1,0 };
guVector Entity::s_axixZ = (guVector){ 0,0,1 };
#endif // !ENTITY_H_
