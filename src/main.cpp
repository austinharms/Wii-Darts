#include <stdint.h>
#include <gctypes.h>
#include <new>

#include "core/Logger.h"
#include "TestEntity.h"
#include "RenderEntity.h"
#include "core/Renderer.h"
#include "core/Font.h"
#include "core/Entity.h"

int main(void) {
	using namespace wiidarts;
	// ensure our renderer is setup first
	Renderer& renderer = Renderer::getInstance();

	// load and setup default font
	Font* font = new(std::nothrow) Font(Prefabs::DefaultFontAtlas);
	if (!font) Logger::fatal("Failed to create defaultFont");
	font->setSize(20);
	font->setColor(0xff0000ff);

	// create master/parent entity
	// all entitys should be a child of this one
	Entity* masterEntity = new(std::nothrow) Entity();
	if (!masterEntity) Logger::fatal("Failed to create master entity");

	TestEntity* testEntity = new(std::nothrow) TestEntity();
	if (!testEntity) Logger::fatal("Failed to create dart entity");
	//masterEntity->addChild(*testEntity);

	// limit the scope of this entity
	{
		RenderEntity* backWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
		if (!backWall) Logger::fatal("Failed to create backWall");
		backWall->getTransform().rotate({ 0, M_PI, 0 });
		backWall->getTransform().translate({ 0,0,2.75f });
		backWall->getTransform().scale({ 5, 5, 5 });
		masterEntity->addChild(*backWall);

		RenderEntity* dartBoardEntity = new(std::nothrow) RenderEntity(Prefabs::DartBoardMesh, Prefabs::DartBoardTexture);
		if (!dartBoardEntity) Logger::fatal("Failed to create dart board entity");
		// set board rotation
		dartBoardEntity->getTransform().rotate({ -M_PI / 2.0f, M_PI, 0 });
		// scale back down 
		// add it to the backWall entity and forget about it
		backWall->addChild(*dartBoardEntity);
		dartBoardEntity->drop();
		backWall->drop();
	}

	while (true)
	{
		masterEntity->update();
		font->setCursor(font->getSize(), renderer.getScreenHeight() - font->getSize());
		font->drawText("Test text\nWW a new line!\nhow well will this work with #?\n0123456789\n!@#$%^&*(){}|\\\"';:<>=_-+");
		renderer.swapFrameBuffers();
	}

	testEntity->drop();
	font->drop();
	return 0;
}