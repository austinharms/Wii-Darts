#include "Scene.h"

#include <new>

#define _USE_MATH_DEFINES
#include <math.h>

#include "../core/Renderer.h"
#include "../Prefabs.h"
#include "../core/Entity.h"
#include "../core/RenderEntity.h"
#include "../core/Logger.h"

namespace wiidarts {
	namespace Scene {
		Entity* _loadedSceneParent = nullptr;
		const float DART_BOARD_HEIGHT = 1.73f;
		const float DART_BOARD_DISTANCE = 2.44f;

		void ClearScene() {
			if (_loadedSceneParent != nullptr) {
				if (_loadedSceneParent->getParent())
					_loadedSceneParent->getParent()->removeChild(*_loadedSceneParent);
				_loadedSceneParent->drop();
				_loadedSceneParent = nullptr;
			}
		}

		bool CreateParentEntity() {
			_loadedSceneParent = new(std::nothrow) Entity();
			if (!_loadedSceneParent) return false;
			Renderer::getInstance().getMasterEntity().addChild(*_loadedSceneParent);
			return true;
		}

		bool CreateDartboard() {
			RenderEntity* board = new(std::nothrow) RenderEntity(Prefabs::DartBoardMesh, Prefabs::DartBoardTexture);
			if (!board) return false;
			Transform& t = board->getTransform();
			t.setPosition({ 0, DART_BOARD_HEIGHT, DART_BOARD_DISTANCE });
			t.rotate({ 0, M_PI/2, 0 });
			_loadedSceneParent->addChild(*board);
			return true;
		}

		bool CreateBrickRoom() {
			Entity* room = new(std::nothrow) Entity();
			if (!room) {
				Logger::error("Failed to create room entity");
				return false;
			}

			room->getTransform().scale({ DART_BOARD_DISTANCE, DART_BOARD_DISTANCE, DART_BOARD_DISTANCE });
			room->getTransform().setPosition({ 0, DART_BOARD_DISTANCE, 0 });
			_loadedSceneParent->addChild(*room);

			RenderEntity* backWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
			if (!backWall) {
				Logger::error("Failed to create backWall entity");
				return false;
			}

			backWall->getTransform().rotate({ 0, M_PI, 0 });
			backWall->getTransform().translate({ 0, 0, 1 });
			room->addChild(*backWall);
			backWall->drop();


			RenderEntity* leftWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
			leftWall->getTransform().rotate({ 0, -M_PI / 2.0f, 0 });
			leftWall->getTransform().translate({ 1, 0, 0 });
			if (!backWall) {
				Logger::error("Failed to create leftWall entity");
				return false;
			}

			room->addChild(*leftWall);
			leftWall->drop();

			RenderEntity* rightWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
			rightWall->getTransform().rotate({ 0, M_PI / 2.0f, 0 });
			rightWall->getTransform().translate({ -1, 0, 0 });
			if (!backWall) {
				Logger::error("Failed to create rightWall entity");
				return false;
			}

			room->addChild(*rightWall);
			rightWall->drop();

			RenderEntity* topWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
			topWall->getTransform().rotate({ M_PI / 2.0f, 0, 0 });
			topWall->getTransform().translate({ 0, 1, 0 });
			if (!backWall) {
				Logger::error("Failed to create topWall entity");
				return false;
			}

			room->addChild(*topWall);
			topWall->drop();

			RenderEntity* bottomWall = new(std::nothrow) RenderEntity(Prefabs::WallMesh, Prefabs::WallTexture);
			bottomWall->getTransform().rotate({ -M_PI / 2.0f, 0, 0 });
			bottomWall->getTransform().translate({ 0, -1, 0 });
			if (!backWall) {
				Logger::error("Failed to create bottomWall entity");
				return false;
			}

			room->addChild(*bottomWall);
			bottomWall->drop();

			room->drop();
			return true;
		}

		bool LoadScene(Scene scene)
		{
			ClearScene();
			if (scene == Scene::NONE) return true;
			if (!(CreateParentEntity() && CreateDartboard())) {
				ClearScene();
				return false;
			}

			bool createdScene;
			switch (scene)
			{
			case DART_BOARD:
				createdScene = true;
				break;

			case BRICK_ROOM:
				createdScene = CreateBrickRoom();
				break;

			default:
				Logger::error("Scene::LoadScene Failed to load unknown scene");
				createdScene = false;
				break;
			}

			if (!createdScene) ClearScene();
			return createdScene;
		}
	}
}
