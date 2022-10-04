#include <stdint.h>
#include <atomic>

#ifndef WIIDARTS_REFCOUNT_H_
#define WIIDARTS_REFCOUNT_H_
namespace wiidarts {
	class RefCount
	{
	public:
		inline RefCount();
		inline virtual ~RefCount();

		// increase the ref count by one
		// this insures it will not be deleted until it is dropped
		inline void grab();

		// decrease the ref count by one
		// returns true if the new ref count is 0 and the object was deleted
		inline virtual bool drop();

		// returns the ref count of the RefCount
		inline uint16_t getRefCount() const;

	private:
		std::atomic<uint16_t> _refCount;
	};
}

#include "RefCount.inl"
#endif // !WIIDARTS_REFCOUNT_H_
