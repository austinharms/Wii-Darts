#include <stdint.h>
#include <gctypes.h>
#include <new>

// file system includes
#include <fat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "core/Logger.h"
#include "TestEntity.h"
#include "RenderEntity.h"
#include "core/Renderer.h"
#include "core/Font.h"
#include "core/Entity.h"
#include "core/IO.h"

int main(int argc, char* argv[]) {
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

	//TestEntity* testEntity = new(std::nothrow) TestEntity();
	//if (!testEntity) Logger::fatal("Failed to create dart entity");
	//masterEntity->addChild(*testEntity);

	//// limit the scope of the entities
	//// add it to the master entity and forget about it
	//{
	//	// this should not be a RenderEntity but we need to access the transform
	//	RenderEntity* room = new(std::nothrow) RenderEntity(nullptr, nullptr);
	//	if (!room) Logger::fatal("Failed to create room entity");
	//	room->getTransform().translate({ 0, 0, -5 + 2.44f });
	//	masterEntity->addChild(*room);

	//	RenderEntity* backWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
	//	if (!backWall) Logger::fatal("Failed to create backWall entity");
	//	backWall->getTransform().rotate({ 0, M_PI, 0 });
	//	backWall->getTransform().translate({ 0, 0, 5 });
	//	backWall->getTransform().scale({ 5, 5, 5 });
	//	room->addChild(*backWall);

	//	RenderEntity* dartBoard = new(std::nothrow) RenderEntity(Prefabs::DartBoardMesh, Prefabs::DartBoardTexture);
	//	if (!dartBoard) Logger::fatal("Failed to create dartBoard entity");
	//	// set board rotation
	//	dartBoard->getTransform().rotate({ 0, -M_PI/2.0f, 0 });
	//	// scale back down to original size
	//	dartBoard->getTransform().scale({ 0.2f, 0.2f, 0.2f });
	//	backWall->addChild(*dartBoard);
	//	dartBoard->drop();
	//	backWall->drop();

	//	RenderEntity* leftWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
	//	leftWall->getTransform().scale({ 5, 5, 5 });
	//	leftWall->getTransform().rotate({ 0, -M_PI / 2.0f, 0 });
	//	leftWall->getTransform().translate({ 5, 0, 0 });
	//	if (!leftWall) Logger::fatal("Failed to create leftWall entity");
	//	room->addChild(*leftWall);
	//	leftWall->drop();

	//	RenderEntity* rightWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
	//	rightWall->getTransform().scale({ 5, 5, 5 });
	//	rightWall->getTransform().rotate({ 0, M_PI / 2.0f, 0 });
	//	rightWall->getTransform().translate({ -5, 0, 0 });
	//	if (!rightWall) Logger::fatal("Failed to create rightWall entity");
	//	room->addChild(*rightWall);
	//	rightWall->drop();

	//	RenderEntity* topWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
	//	topWall->getTransform().scale({ 5, 5, 5 });
	//	topWall->getTransform().rotate({ M_PI / 2.0f, 0, 0 });
	//	topWall->getTransform().translate({ 0, 5, 0 });
	//	if (!topWall) Logger::fatal("Failed to create topWall entity");
	//	room->addChild(*topWall);
	//	topWall->drop();

	//	RenderEntity* bottomWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
	//	bottomWall->getTransform().scale({ 5, 5, 5 });
	//	bottomWall->getTransform().rotate({ -M_PI / 2.0f, 0, 0 });
	//	bottomWall->getTransform().translate({ 0, -5, 0 });
	//	if (!bottomWall) Logger::fatal("Failed to create bottomWall entity");
	//	room->addChild(*bottomWall);
	//	bottomWall->drop();
	//	room->drop();
	//}

	IO& io = IO::getInstance();

	DIR* pdir;
	struct dirent* pent;
	struct stat statbuf;

	while (true)
	{
		masterEntity->update();
		font->setCursor(font->getSize(), renderer.getScreenHeight() - font->getSize());

		font->drawText("Dir List:\n");
		if(argc >= 1)
			font->drawText(argv[0]);
		pdir = opendir("./");
		if (pdir) {
			while ((pent = readdir(pdir)) != nullptr)
			{
				stat(pent->d_name, &statbuf);
				if (S_ISDIR(statbuf.st_mode)) {
					font->drawText("dir: ");
				}
				else if (!(S_ISDIR(statbuf.st_mode))) {
					font->drawText("file: ");
				}
				else {
					font->drawText("other: ");
				}

				font->drawText(pent->d_name);
				font->drawText("\n");
			}

			font->drawText("END");
			closedir(pdir);
		}
		else {
			font->drawText("Open Failed");
		}

		io.updateInputs();
		renderer.swapFrameBuffers();
	}

	//testEntity->drop();
	font->drop();
	return 0;
}