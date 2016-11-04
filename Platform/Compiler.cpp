#include "Compiler.h"

#include "Platform.h"

static const char *compilerPath = "cl.exe";

static const char *getCompilerPath() {
	return compilerPath;
}

static bool runCommand(const char *command, FILE *errors) {
	return Platform::Execute(command) == 0;
}

bool Compiler::CompileToObjectFile(char *sourcePath, char *outputPath, FILE *errors) {
	const char *compilerPath = getCompilerPath();

	//Validate arguments
	if (!sourcePath || !outputPath || !compilerPath) {
		return false;
	}
	
	int sourcePathLen = strnlen(sourcePath, 1024);
	int outputPathLen = strnlen(outputPath, 1024);
	int compilerPathLen = strnlen(compilerPath, 1024);

	if (sourcePathLen == 0 || outputPathLen == 0 || compilerPathLen == 0) {
		return false;
	}

	//Compilation flags
	const char *flags = " /GR- /GX- /fp:fast /arch:SSE2 /c /Fo ";
	int flagsLen = strlen(flags);

	//Fill buffer with: {CompilerPath}{Flags}{OutputPath} {SourcePath}
	char *buff = (char*)malloc(4096);
	char *buffPos = buff;
	memcpy(buffPos, compilerPath, compilerPathLen);
	buffPos += compilerPathLen;
	
	memcpy(buffPos, flags, flagsLen);
	buffPos += flagsLen;

	memcpy(buffPos, outputPath, outputPathLen);
	buffPos += outputPathLen;

	*buffPos = ' ';
	buffPos++;

	memcpy(buffPos, sourcePath, sourcePathLen);
	buffPos += sourcePathLen;

	*buffPos = 0; //Null terminate

	bool result = runCommand(buff, errors);
	free(buff);
	return result;
}

bool Compiler::LinkToDynamicLibrary(const std::vector<char*> &objectFiles, char *outputPath, FILE *errors) {
	return false;
}
