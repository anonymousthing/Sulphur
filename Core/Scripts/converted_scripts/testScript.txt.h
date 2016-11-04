#include "Script.h"

class SCRIPT_testScript : public Script {
public:
virtual void Init() {
Game &engine = *Script::getEngine();
engine.registerTickHandler();
}


void onTick(int deltaMS) {
Game &engine = *Script::getEngine();
engine.testScriptMovement();
}


};


