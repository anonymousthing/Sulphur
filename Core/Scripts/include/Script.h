#pragma once

#ifdef ___SCRIPT
#define SCRIPT_API	__declspec(dllimport)
#else
#define SCRIPT_API	__declspec(dllexport)
#endif

//--------------------------------------------------
//------------ Game Engine Dependencies ------------
//--------------------------------------------------

//Include any headers that Scripts need here.
//These headers must include every type/method used in Types.txt.
#include "Game.h"

class Script;
#include <string>
#include <functional>

//--------------------------------------------------
//------------------ Registration ------------------
//--------------------------------------------------
void do_RegisterScript(std::string &name, Script *script);

template<typename ScriptTy>
struct RegisterScript {
	RegisterScript() { do_RegisterScript(new ScriptTy()); }
};

//Usage:
//RegisterScript<ScriptTy> x;

//--------------------------------------------------
//------------ Common Script Base Class ------------
//--------------------------------------------------

//This is the baseclass that all Scripts extend.
//This allows the game to create/destroy scripts and pass them around.
//
//The engine, when given an instance, should call setEngine(this) before calling
//init on all Scripts.
class Script {
private:
	Game *engine;

protected:
	Game *getEngine() { return engine; }
	
public:
	Script() : engine(nullptr) { }
	virtual ~Script() { engine = nullptr; }

	virtual void Init() { }
	virtual void Destroy() { }

	void setEngine(Game *engine) { this->engine = engine; }
};
