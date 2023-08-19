#ifndef DARTS_ALLOCATOR_H_
#define DARTS_ALLOCATOR_H_
#include "Ints.h"

// A stack like allocator for allocating blocks of memory
// Note the head and/or tail must be cleared to "free" any block of memory
class Allocator
{
public:
	Allocator();
	~Allocator();

	// Allocates memory between start and end (inclusive, exclusive)
	void Init(void* start, void* end);
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
	void RestoreHead(void* head);
	void RestoreTail(void* tail);

private:
	void* m_stackStart;
	void* m_stackEnd;
	void* m_stackHead;
	void* m_stackTail;
};
#endif // !DARTS_ALLOCATOR_H_