#include "ScriptManager.h"
#include "ScriptLoader.h"

ScriptManager::ScriptManager(Game *game) : game(game) {

}

void ScriptManager::loadScripts() {
	std::vector<Script *> scripts = ScriptLoader::loadScripts();


}
