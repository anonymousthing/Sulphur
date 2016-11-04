#pragma once

#include "Log.h"
#include "FontLoader.h"
#include "GraphicsHandles.h"

#include <vector>
#include <utility>

#include <glew\glew.h>

class ResLoader {
private:
	//var-args helpers for loadShaders.
	static ShaderHandle _loadShaders(std::vector<std::pair<const char *, GLenum>> shaders,
		const char *shader1, GLenum shaderType) {
		//Last set, call loadShaders
		shaders.push_back(std::make_pair(shader1, shaderType));
		return loadShaders(shaders);
	}

	template<typename... Args>
	static ShaderHandle _loadShaders(std::vector<std::pair<const char *, GLenum>> shaders,
		const char *shader1, GLenum shaderType, Args... args) {
		//Push the next set and keep going.
		shaders.push_back(std::make_pair(shader1, shaderType));
		return _loadShaders(shaders, args...);
	}

public:
	static TextureHandle loadDDS(const char *imagepath);
	static GLuint loadBMP_custom(const char *imagepath);
	static TextureHandle ResLoader::loadPNG(const char *path);
	static TextureHandle ResLoader::loadTexture(const char *path);

	//Compiles the given shader.
	static GLuint compileShader(const char *shaderPath, GLenum shaderType);

	//Links the given shaders together.
	static ShaderHandle linkShaders(std::vector<GLuint> shaders);

	//var-args version of loadShaders.
	template<typename... Args>
	static ShaderHandle loadShaders(const char *shader1, GLenum shaderType, Args... args) {
		//Create the vector and push the first set.
		std::vector<std::pair<const char *, GLenum>> shaders;
		shaders.push_back(std::make_pair(shader1, shaderType));

		return _loadShaders(shaders, args...);
	}

	//Loads & links the given shaders together.
	static ShaderHandle loadShaders(std::vector<std::pair<const char *, GLenum>> shaders);

	static Font *loadFont(const char *fontDescriptionFile) {
		return FontLoader::loadFont(fontDescriptionFile);
	}
};
