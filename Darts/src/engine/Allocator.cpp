#include "engine/Allocator.h"
#include <malloc.h>

Allocator::Allocator(size_t stackSize) {
	m_stackStart = malloc(stackSize);
	m_stackEnd = (void*)(((uint8_t*)m_stackStart) + stackSize);
	m_stackHead = m_stackStart;
	m_stackTail = m_stackEnd;
}

Allocator::~Allocator() {
	if (m_stackStart) free(m_stackStart);
}

bool Allocator::GetStackValid() const
{
	return m_stackStart != nullptr;
}

size_t Allocator::GetStackUsed() const
{
	return GetStackSize() - GetStackFree();
}

size_t Allocator::GetStackSize() const
{
	return (size_t)m_stackEnd - (size_t)m_stackStart;
}

size_t Allocator::GetStackFree() const
{
	return (size_t)m_stackTail - (size_t)m_stackHead;
}

bool Allocator::OwnsMemory(void* block)
{
	return block >= m_stackStart && block < m_stackEnd;
}

void Allocator::ClearAllocations()
{
	m_stackHead = m_stackStart;
	m_stackTail = m_stackEnd;
}

void Allocator::ClearHead()
{
	m_stackHead = m_stackStart;
}

void Allocator::ClearTail()
{
	m_stackTail = m_stackEnd;
}

void* Allocator::Allocate(size_t size)
{
	if (size > GetStackFree()) return nullptr;
	void* blockStart = m_stackHead;
	m_stackHead = (void*)(((uint8_t*)m_stackHead) + size);
	return blockStart;
}

void* Allocator::AllocateTail(size_t size)
{
	if (size > GetStackFree()) return nullptr;
	m_stackTail = (void*)(((uint8_t*)m_stackTail) - size);
	return m_stackTail;
}
