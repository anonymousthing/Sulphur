#pragma once

#include "Graphics.h"
#include "SulphurWorld.h"
#include "Sprite.h"
#include "ModelLoader.h"
#include "ScriptManager.h"
#include "ScreenManager.h"
#include "InputManager.h"
#include "FPS.h"

class Game {
	GLFWwindow *window;
	ScriptManager *scriptManager;

    void addTestTriangles(Screen *screen);
	void processInput(GLFWwindow *window, KeyboardInput *kb, Vector3 *cameraPos, Vector3 *cameraTarget, float movespeed);

public:
	SulphurWorld *world;

	Graphics *graphics;
	InputManager *inputManager;
	ScreenManager *screenManager;

	Game();
	void run();
	void SCRIPT_API testScriptMovement();

	SulphurWorld *getWorld() {
		return world;
	}

	//TODO: This should be removed -- when Scripts can access world this won't be needed.
	void SCRIPT_API registerTickHandler(TickHandler handler) {
		world->registerTickHandler(handler);
	}
};
