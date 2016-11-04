#pragma once

#include "Script.h"

class Game;

class ScriptManager {

private:
	Game *game;

public:
	ScriptManager(Game *game);

	void loadScripts();
};
