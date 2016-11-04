#include "ScriptLoader.h"

#include "Platform.h"
#include "Log.h"

#include <stdlib.h>

static Log logger("ScriptLoader");


const char *ScriptLoader::getScriptConverterCommandLine() {
	return "ScriptConverter.exe " SCRIPT_DIR "%s /out " SCRIPT_DIR SCRIPT_CPP_DIR "%s.h";
}

const char *ScriptLoader::getScriptCompilerCommandLine() {
	return "clang.exe -c "; //TODO:
}

std::vector<char*> ScriptLoader::loadScriptIndex() {
	std::vector<char*> scripts;

	const char *scriptIndex = SCRIPT_DIR SCRIPT_INDEX_PATH;
	logger.info("Loading script index from \"%s\"", scriptIndex);

	FILE *file = fopen(scriptIndex, "r");

	if (file == nullptr) {
		logger.fatal("Could not open Script Index.");
	}
	
	char lineBuff[512];
	while (fgets(lineBuff, 512, file)) {
		//We have a line
		size_t len = strlen(lineBuff);

		//Copy it to a new char*
		//This pointer will eventually be owned by the Script we load.
		char *scriptName = (char*)malloc(len + 1);
		memcpy(scriptName, lineBuff, len + 1);

		scripts.push_back(scriptName);
	}

	fclose(file);

	return scripts;
}

bool ScriptLoader::convertScript(const char *scriptName) {
	//Construct our invocation string

	const char *converterFmt = getScriptConverterCommandLine();
	//Should take scriptName, outputName (without trailing '.h') as format.

	size_t totalLen = strlen(converterFmt) + (strlen(scriptName) * 2) + 16;
	char *invocation = (char*)malloc(totalLen + 1);

	snprintf(invocation, totalLen, converterFmt, scriptName, scriptName);

	logger.info("Converting script \"%s\"", scriptName);
	logger.info("Using: \"%s\"...", invocation);

	int returnCode = Platform::Execute(invocation);

	free(invocation);

	return returnCode == 0;
}

bool ScriptLoader::compileScript(const char *scriptName) {
	const char *compilerFmt = getScriptCompilerCommandLine();

	size_t totalLen = strlen(compilerFmt) + strlen(scriptName) + 16;
	char *invocation = (char*)malloc(totalLen);
	//FIXME: snprintf?
	_snprintf(invocation, totalLen, compilerFmt, scriptName);

	logger.info("Compiling script \"%s\"", scriptName);
	logger.info("Using: \"%s\"...", invocation);

	int returnCode = 0;// Platform::Execute(invocation);
	
	free(invocation);

	return false;
}

std::vector<Script *> ScriptLoader::loadScripts() {
	std::vector<Script *> scripts;

	std::vector<char *> scriptNames = loadScriptIndex();

	for (auto si = scriptNames.begin(), se = scriptNames.end(); si != se; ++si) {
		char *scriptName = *si;

		//TODO: Check to see if we already have it compiled.
		if (!convertScript(scriptName)) {
			free(scriptName);
			continue;
		}

		if (!compileScript(scriptName)) {
			free(scriptName);
			continue;
		}

		//Now load the script
		Script *script = loadScript(scriptName);
		if (!script) {
			free(scriptName);
			continue;
		}

		scripts.push_back(script);
	}

	return scripts;
}

Script *ScriptLoader::loadScript(char *scriptName) {
	logger.info("Loading script \"%s\"", scriptName);

	return nullptr;
}
