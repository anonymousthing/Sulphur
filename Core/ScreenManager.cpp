#include "ScreenManager.h"
#include "Log.h"

#include <functional>

#include <algorithm>

static Log logger("ScreenManager");

ScreenManager::ScreenManager(Graphics *graphics, InputManager *inputManager)
	: graphics(graphics), inputManager(inputManager) {
	inputManager->setOnKeyDown(std::bind(&ScreenManager::onKeyDown, this, std::placeholders::_1, std::placeholders::_2));
	inputManager->setOnKeyUp(std::bind(&ScreenManager::onKeyUp, this, std::placeholders::_1, std::placeholders::_2));	
}

ScreenManager::~ScreenManager() {
	for (auto si = screens.begin(), se = screens.end(); si != se; ++si) {
		delete *si;
	}
}

void ScreenManager::addScreen(Screen *screen) {
	auto si = std::find(screens.begin(), screens.end(), screen);
	if (si != screens.end()) {
		logger.error("Attempted to addScreen screen which already exists.");
		//Be nice and pretend to work
		bringToFront(screen);
		return;
	}

	screens.push_back(screen);
}

void ScreenManager::destroyScreen(Screen *screen) {
	auto si = std::find(screens.begin(), screens.end(), screen);
	if (si == screens.end()) {
		logger.error("Attempted to destroyScreen screen which does not exist.");
		return;
	}

	screens.erase(si);
	delete screen;
}

void ScreenManager::bringToFront(Screen *screen) {
	auto si = std::find(screens.begin(), screens.end(), screen);
	if (si == screens.end()) {
		logger.error("Attempted to bringToFront screen which does not exist.");
		//Be nice and pretend to work
		addScreen(screen);
		return;
	}

	screens.erase(si);
	screens.push_back(screen);
}

void ScreenManager::sendToBack(Screen *screen) {
	auto si = std::find(screens.begin(), screens.end(), screen);
	if (si == screens.end()) {
		logger.error("Attempted to sendToBack screen which does not exist.");
		return;
	}

	screens.erase(si);
	screens.insert(screens.begin(), screen);
}

void ScreenManager::onKeyDown(Keys key, const KeyInfo *keyInfo) {
	//Move beginning -> end
	for (auto si = screens.rbegin(), se = screens.rend(); si != se; ++si) {
		Screen *screen = *si;
		bool updateNext = screen->onKeyDown(key, keyInfo);

		//Check to see if we should update the next screen.
		if (!updateNext) {
			break;
		}
	}
}

void ScreenManager::onKeyUp(Keys key, const KeyInfo *keyInfo) {
	//Move beginning -> end
	for (auto si = screens.rbegin(), se = screens.rend(); si != se; ++si) {
		Screen *screen = *si;
		bool updateNext = screen->onKeyUp(key, keyInfo);

		//Check to see if we should update the next screen.
		if (!updateNext) {
			break;
		}
	}
}

void ScreenManager::update() {
	//Move beginning -> end
	for (auto si = screens.rbegin(), se = screens.rend(); si != se; ++si) {
		Screen *screen = *si;
		bool updateNext = screen->update(inputManager);

		//Check to see if we should update the next screen.
		if (!updateNext) {
			break;
		}
	}
}

void ScreenManager::draw(int delta) {
	graphics->beginScene();

	if (screens.size() == 0) {
		graphics->endScene();
		return;
	}

	//Find last non-overlay
	auto si = screens.rbegin();
	for (auto se = screens.rend(); si != se; ++si) {
		Screen *screen = *si;
		if (!screen->isOverlay) {
			break;
		}
	}
	if (si == screens.rend()) {
		--si; //Step back from end.
	}

	//si is the first screen to draw.
	while (true) {
		Screen *screen = *si;
		screen->draw(delta);

		if (si == screens.rbegin()) {
			break;
		}
		--si;
	}
    graphics->endScene();
}
