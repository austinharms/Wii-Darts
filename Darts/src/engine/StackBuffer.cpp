#include "engine/StackBuffer.h"
#include "engine/Engine.h"

StackBuffer::StackBuffer(size_t size) : buffer(Engine::StackBufferAllocate(size)) {}

StackBuffer::~StackBuffer() {
	if (buffer) Engine::StackBufferRestore(buffer);
}