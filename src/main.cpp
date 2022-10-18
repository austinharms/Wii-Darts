#include <stdint.h>
#include <gctypes.h>
#include <new>
#include <stdio.h>

#include "core/Logger.h"
#include "TestEntity.h"
#include "core/RenderEntity.h"
#include "core/Renderer.h"
#include "core/Font.h"
#include "core/Entity.h"
#include "core/IO.h"
#include "core/BasicFontAtlas.h"
#include "darts/Scene.h"

int main(void) {
	using namespace wiidarts;
	// ensure our renderer is setup first and also create IO Instance 
	Renderer& renderer = Renderer::getInstance();
	// set to dartboard height
	// why is this negative?
	renderer.getCameraTransform().translate({ 0, -1.73f, 0 });
	Entity& masterEntity = renderer.getMasterEntity();
	IO& io = IO::getInstance();
	io.setWiimoteInputType(IO::BUTTONS_ACCELEROMETER_POINTER);

	// load and setup default font
	Font* font = new(std::nothrow) Font(Prefabs::DefaultFontAtlas);
	if (!font) Logger::fatal("Failed to create defaultFont");
	font->setSize(30);
	font->setColor(0xffffffff);

	RenderEntity* dart = new(std::nothrow) RenderEntity(Prefabs::DartMesh, Prefabs::DartTexture, 0xffffff90);
	if (!dart) Logger::fatal("Failed to create dart entity");
	dart->getTransform().rotate({ 0, M_PI, 0 });
	masterEntity.addChild(*dart);

	renderer.setFOV(90, false);
	Scene::LoadScene(Scene::BRICK_ROOM);

	char buf[1024];
	while (true)
	{
		masterEntity.update();
		font->setCursor(font->getSize() * 2, renderer.getScreenHeight() - font->getSize() * 2);
		float x;
		float y;
		guVector cursorVector;
		if (io.getControllerIRScreenPos(0, &x, &y) && io.getControllerIRVector(0, cursorVector)) {
			sprintf(buf, "IR: %4.2f, %4.2f\nVec: %4.2f, %4.2f, %4.2f", x, y, cursorVector.x, cursorVector.y, cursorVector.z);
			font->drawText(buf);
			font->setCursor(x, y);
			font->drawText("O");
			float scale = 2.44f * (1 / cursorVector.z);
			guVecScale(&cursorVector, &cursorVector, scale);
			guVector pos = renderer.getCameraTransform().getPosition();
			// camera y is inverted?
			pos.y = -pos.y;
			guVecAdd(&pos, &cursorVector, &pos);
			dart->getTransform().setPosition(pos);
		}

		io.updateInputs();
		io.rumbleController(0, io.getControllerButtonDown(0, WPAD_BUTTON_B));
		if (io.getControllerButtonPressed(0, WPAD_BUTTON_A)) renderer.setFOV(90);
		if (io.getControllerButtonReleased(0, WPAD_BUTTON_A)) renderer.setFOV(30);
		renderer.swapFrameBuffers();
	}

	Scene::LoadScene(Scene::NONE);
	dart->drop();
	font->drop();
	return 0;
}