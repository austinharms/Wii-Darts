#ifndef DARTS_STACK_BUFFER_H_
#define DARTS_STACK_BUFFER_H_
#include "Core.h"

struct StackBuffer
{
public:
	StackBuffer(size_t size);
	~StackBuffer();
	const void* buffer;
};
#endif // !DARTS_STACK_BUFFER_H_
