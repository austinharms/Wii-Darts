#include <stdint.h>
#include <gctypes.h>
#include <new>

#include "TestEntity.h"
#include "core/Renderer.h"

int main(void) {
	using namespace wiidarts;
	Renderer& renderer = Renderer::getInstance();
	TestEntity* testEntity = new(std::nothrow) TestEntity();
	if (!testEntity) return -1;
	while (true)
	{
		testEntity->update();
		renderer.swapFrameBuffers();
	}

	testEntity->drop();
	return 0;
}