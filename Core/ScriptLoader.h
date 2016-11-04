#pragma once

#include "Script.h"

#include <vector>

#define SCRIPT_DIR				"scripts\\"

//All of the below are paths relative to 'SCRIPT_DIR'
#define SCRIPT_INDEX_PATH		"ScriptIndex.txt"
#define SCRIPT_OUTPUT_DIR		"compiled_scripts\\"
#define SCRIPT_CPP_DIR			"converted_scripts\\"

class ScriptLoader {
private:
	//The returned 
	static std::vector<char*> loadScriptIndex();

	//Returns the script converter command line as a format string.
	static const char *getScriptConverterCommandLine();
	 
	//Returns the script compiler command line as a format string.
	static const char *getScriptCompilerCommandLine();

	static bool convertScript(const char *scriptName);
	static bool compileScript(const char *scriptName);

public:
	//Loads all Scripts from scriptIndex
	static std::vector<Script *> loadScripts();

	//Invokes the ScriptConverter before compiling the script.
	static Script *loadScript(char *scriptName);
};
