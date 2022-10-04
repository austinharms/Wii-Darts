#include <stdint.h>
#include <gctypes.h>
#include <new>

#include "TestEntity.h"
#include "core/Renderer.h"
#include "BasicFontAtlas.h"
#include "core/Font.h"

int main(void) {
	using namespace wiidarts;
	Renderer& renderer = Renderer::getInstance();
	BasicFontAtlas& testAtlas = BasicFontAtlas::getInstance();
	Font* testFont = new(std::nothrow) Font(&testAtlas);
	if (!testFont) return -1;
	testFont->setSize(50);
	testFont->setColor(0xff0000ff);
	TestEntity* testEntity = new(std::nothrow) TestEntity();
	if (!testEntity) return -1;
	while (true)
	{
		testEntity->update();
		testFont->setCursor(testFont->getSize(), renderer.getScreenHeight() - testFont->getSize());
		testFont->drawText("Test text\nWW a new line!\nhow well will this work with #?\n0123456789\n!@#$%^&*(){}|\\\"';:<>=_-+");
		renderer.swapFrameBuffers();
	}

	testEntity->drop();
	testFont->drop();
	return 0;
}