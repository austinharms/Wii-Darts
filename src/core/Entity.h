#include <stdint.h>

#include "Transform.h"
#include "RefCount.h"

#ifndef WIIDARTS_ENTITY_H_
#define WIIDARTS_ENTITY_H_
namespace wiidarts {
	struct EntityNode;
	class Entity : public RefCount
	{
	public:
		Entity();
		virtual ~Entity();

		// returns the Entity Transform
		const Transform& getTransform() const;

		// returns the Entity Transform
		Transform& getTransform();

		// calls the onUpdate method of this Entity and all of its children
		void update();

		// returns the child Entity at index
		// if index >= child count returns null
		Entity* getChild(uint16_t index = 0) const;

		// returns the number of child Entity this Entity has
		uint16_t getChildCount() const;

		// returns the Entity's parent Entity
		// Note: this may be null if there is no parent
		Entity* getParent() const;

		// adds a child Entity to this Entity
		// if index > child count child will be inserted at the end of the child list
		// returns the index the child was inserted at, -1 if it failed to add the child
		int32_t addChild(Entity& child, uint16_t index = 0);

		// attempts to remove the child Entity, returns true on success 
		// returns false if the child was not a child of this Entity
		bool removeChild(Entity& child);

		// attempts to remove the child at index, returns true on success 
		// returns false if index >= child count
		bool removeChild(uint16_t index);

		// removes all child Entity's
		void clearChildren();

	protected:
		Entity* _parent;
		EntityNode* _children;
		Transform _transform;
		uint16_t _childCount;

		inline virtual void onUpdate();
	};

	struct EntityNode
	{
		EntityNode* Next;
		Entity& Value;
	};
}
#endif // !WIIDARTS_ENTITY_H_
