#ifndef ENTITY_H_
#define ENTITY_H_
#include "Vector3f.h"

class Entity
{
public:
	Entity(Vector3f pos) : Pos(pos) { }
	Entity() : Pos(0) { }
	virtual void Update() = 0;
	Vector3f Pos;
};
#endif // !ENTITY_H_
