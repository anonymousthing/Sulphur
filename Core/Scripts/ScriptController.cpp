//The compiler will force the scripts we want to be included, through the command line.
//This is the 'carrier' for the DLLs we create

#include "Script.h"

#include <string>
#include <vector>
#include <map>

static std::map<std::string, Script*> scripts;

void do_RegisterScript(const std::string &name, Script *script) {
	scripts.insert(std::make_pair(name, script));
}

std::vector<std::string> SCRIPT_API GetScriptNames() {
	std::vector<std::string> scriptNames;

	for (auto si = scripts.begin(), se = scripts.end(); si != se; ++si) {
		const std::string &key = si->first;

		scriptNames.push_back(key);
	}

	return scriptNames;
}

bool SCRIPT_API InitialiseScript(const char *scriptName, Game *game) {
	std::string name(scriptName);

	auto si = scripts.find(name);
	if (si == scripts.end()) {
		return false;
	}

	Script *script = si->second;
	script->setEngine(game);
	script->Init();

	return true;
}
