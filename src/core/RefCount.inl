#include "RefCount.h"
namespace wiidarts {
	RefCount::RefCount() {
		_refCount = 1;
	}

	RefCount::~RefCount() {}
	
	void RefCount::grab()
	{
		++_refCount;
	}
	
	bool RefCount::drop()
	{
		if (--_refCount == 0) {
			delete this;
			return true;
		}

		return false;
	}
	
	uint16_t RefCount::getRefCount() const
	{
		return _refCount;
	}
}