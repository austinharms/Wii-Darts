#ifndef DARTS_ALLOCATOR_H_
#define DARTS_ALLOCATOR_H_
#include "Ints.h"

namespace darts {
	class Allocator
	{
	public:
		Allocator(size_t stackSize);
		~Allocator();

		bool GetStackValid() const;
		size_t GetStackUsed() const;
		size_t GetStackSize() const;
		size_t GetStackFree() const;
		bool OwnsMemory(void* block);
		void ClearAllocations();
		void ClearHead();
		void ClearTail();
		void* Allocate(size_t size);
		void* AllocateTail(size_t size);

	private:
		void* m_stackStart;
		void* m_stackEnd;
		void* m_stackHead;
		void* m_stackTail;
	};
}
#endif // !DARTS_ALLOCATOR_H_