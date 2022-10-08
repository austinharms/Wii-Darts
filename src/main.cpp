#include <stdint.h>
#include <gctypes.h>
#include <new>

#include "core/Logger.h"
#include "TestEntity.h"
#include "RenderEntity.h"
#include "core/Renderer.h"
#include "core/Font.h"
#include "core/Entity.h"
#include "core/IO.h"

#include "core/BasicFontAtlas.h"

int main(void) {
	using namespace wiidarts;
	// ensure our renderer is setup first and also create IO Instance 
	Renderer& renderer = Renderer::getInstance();
	IO& io = IO::getInstance();

	// load and setup default font
	Font* font = new(std::nothrow) Font(Prefabs::DefaultFontAtlas);
	if (!font) Logger::fatal("Failed to create defaultFont");
	font->setSize(40);
	font->setColor(0xff0000ff);

	// create master/parent entity
	// all entitys should be a child of this one
	Entity* masterEntity = new(std::nothrow) Entity();
	if (!masterEntity) Logger::fatal("Failed to create master entity");

	TestEntity* testEntity = new(std::nothrow) TestEntity();
	if (!testEntity) Logger::fatal("Failed to create dart entity");
	masterEntity->addChild(*testEntity);

	// limit the scope of the entities
	// add it to the master entity and forget about it
	{
		// this should not be a RenderEntity but we need to access the transform
		RenderEntity* room = new(std::nothrow) RenderEntity(nullptr, nullptr);
		if (!room) Logger::fatal("Failed to create room entity");
		room->getTransform().scale({ 2.5f, 2.5f, 2.5f });
		room->getTransform().translate({ 0, 0, -0.06f });
		masterEntity->addChild(*room);

		RenderEntity* backWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
		if (!backWall) Logger::fatal("Failed to create backWall entity");
		backWall->getTransform().rotate({ 0, M_PI, 0 });
		backWall->getTransform().translate({ 0, 0, 1 });
		room->addChild(*backWall);

		RenderEntity* dartBoard = new(std::nothrow) RenderEntity(Prefabs::DartBoardMesh, Prefabs::DartBoardTexture);
		if (!dartBoard) Logger::fatal("Failed to create dartBoard entity");
		// set board rotation
		dartBoard->getTransform().rotate({ 0, -M_PI/2.0f, 0 });
		// scale back down to original size
		dartBoard->getTransform().scale({ 0.4f, 0.4f, 0.4f });
		backWall->addChild(*dartBoard);
		dartBoard->drop();
		backWall->drop();

		RenderEntity* leftWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
		leftWall->getTransform().rotate({ 0, -M_PI / 2.0f, 0 });
		leftWall->getTransform().translate({ 1, 0, 0 });
		if (!leftWall) Logger::fatal("Failed to create leftWall entity");
		room->addChild(*leftWall);
		leftWall->drop();

		RenderEntity* rightWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
		rightWall->getTransform().rotate({ 0, M_PI / 2.0f, 0 });
		rightWall->getTransform().translate({ -1, 0, 0 });
		if (!rightWall) Logger::fatal("Failed to create rightWall entity");
		room->addChild(*rightWall);
		rightWall->drop();

		RenderEntity* topWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
		topWall->getTransform().rotate({ M_PI / 2.0f, 0, 0 });
		topWall->getTransform().translate({ 0, 1, 0 });
		if (!topWall) Logger::fatal("Failed to create topWall entity");
		room->addChild(*topWall);
		topWall->drop();

		RenderEntity* bottomWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
		bottomWall->getTransform().rotate({ -M_PI / 2.0f, 0, 0 });
		bottomWall->getTransform().translate({ 0, -1, 0 });
		if (!bottomWall) Logger::fatal("Failed to create bottomWall entity");
		room->addChild(*bottomWall);
		bottomWall->drop();
		room->drop();
	}

	while (true)
	{
		masterEntity->update();
		font->setCursor(font->getSize() * 2, renderer.getScreenHeight() - font->getSize() * 2);
		font->drawText("Hello World!");
		io.updateInputs();
		renderer.swapFrameBuffers();
	}

	testEntity->drop();
	font->drop();
	return 0;
}