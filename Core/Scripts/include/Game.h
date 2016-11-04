#pragma once

#include <functional>

typedef std::function<void(int)> TickHandler;

class Game {
public:
	void SCRIPT_API testScriptMovement();

	void SCRIPT_API registerTickHandler(TickHandler handler);
};
