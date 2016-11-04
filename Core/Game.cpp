#include "Game.h"
#include "Platform.h"

void Game::testScriptMovement() {
	graphics->testScriptMovement();
}

Game::Game() {
	//Init graphics engine
	graphics = new Graphics();
	graphics->setCamera(new Camera(graphics));
	window = graphics->getWindowHandle();

	//Initialise input manager
	inputManager = new InputManager(window);

	//Initialise our World
	world = new SulphurWorld(graphics);

	//Initalise screen manager
    screenManager = new ScreenManager(graphics, inputManager);

	//Initialise script manager
	scriptManager = new ScriptManager(this);
	scriptManager->loadScripts();
}

void Game::run() {
	float movespeed = 0.15f;

	int delta = 0;
	uint64_t lastFrameTime = 0;
	FPS *fps = new FPS();
	
	char *windowTitle = (char*)malloc(33 * sizeof(char));

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		//Calculate delta
		uint64_t thisFrameTime = Platform::GetElapsedMilliseconds();
		delta = thisFrameTime - lastFrameTime;
		lastFrameTime = thisFrameTime;

		//Get FPS
		snprintf(windowTitle, 32, "Sulphur - %f", fps->frame());

		glfwSetWindowTitle(window, windowTitle);
		//Get input
		screenManager->update();

		//Step physics
		world->tick(delta);

		screenManager->draw(delta);

		//Platform::Sleep(5);
	}

	delete inputManager;
	delete scriptManager;
    delete screenManager;
	delete world;
	delete graphics;
}
