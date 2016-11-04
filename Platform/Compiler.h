#pragma once

#include <stdio.h>
#include <vector>

class Compiler {
public:
	static bool CompileToObjectFile(char *sourcePath, char *outputPath = nullptr, FILE *errors = nullptr);
	static bool LinkToDynamicLibrary(const std::vector<char*> &objectFiles, char *outputPath, FILE *errors = nullptr);
};
