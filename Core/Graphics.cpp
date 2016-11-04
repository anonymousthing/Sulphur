#include "Graphics.h"
#include "Log.h"
#include "ResLoader.h"
#include "GLHandles.h"

#include <stdint.h>
#include <stdlib.h> //malloc
#include "Camera.h"

static Log logger("Graphics");

void Graphics::testScriptMovement() {
	//cameraPos.data[0] += movespeed;
	//cameraTarget.data[0] += movespeed;
}

Graphics::Graphics() {
	initGL();
}

Graphics::~Graphics() {
	glfwTerminate();
}

void Graphics::initGL() {
	if (!glfwInit()) {
		logger.fatal("Failed to initialise GLFW");
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Sulphur", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		logger.fatal("Failed to open GLFW Window");
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		//FIXME: Should we be calling glfwTerminate here too?
		logger.fatal("Failed to initialise GLEW");
	}


	glClearColor(0.28f, 0.24f, 0.54f, 0.0f);

	//Check depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Create program and load shaders
	//Load default 3d shader
	shader3d = ResLoader::loadShaders("vert3d.glsl", GL_VERTEX_SHADER, "frag3d.glsl", GL_FRAGMENT_SHADER);
	if (shader3d.getHandle() == nullptr) {
		logger.fatal("Default 3D shader failed to load.");
	}

	shaderMVP = shader3d.getParameter("MVP");
	shader3d.setActive();

	//Load 2d sprite shader
	shader2d = ResLoader::loadShaders("vert2d.glsl", GL_VERTEX_SHADER, "frag2d.glsl", GL_FRAGMENT_SHADER);
	if (shader2d.getHandle() == nullptr) {
		logger.fatal("Default sprite shader failed to load.");
	}
	shader2dAspectRatio = shader2d.getParameter("aspectRatio");
	shader2dSpritePos = shader2d.getParameter("spritePos");

	//Load 2d sprite shader
	shader3dcolor = ResLoader::loadShaders("vert3dcolor.glsl", GL_VERTEX_SHADER, "frag3dcolor.glsl", GL_FRAGMENT_SHADER);
	if (shader3dcolor.getHandle() == nullptr) {
		logger.fatal("Solid colour 3D shader failed to load.");
	}

	logger.info("\nVendor:\t%s\nGPU:\t%s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	logger.info("\nRunning OpenGL version:\n%s", glGetString(GL_VERSION));
}

void Graphics::setResolution(int width, int height) {
	this->width = width;
	this->height = height;
	pixelWidth = 1.0f / width;
	pixelHeight = 1.0f / height;

	glfwSetWindowSize(window, width, height);

	aspectRatio = (float)width / height;

	projection = Matrix4x4::Perspective(3.1415f * (aspectRatio * (9.0f / 16)) / 3, aspectRatio, 0.1f, 10000.f);
	updateProjectionView();
}

float Graphics::getPixelWidth() {
	return pixelWidth;
}

float Graphics::getPixelHeight() {
	return pixelHeight;
}

void Graphics::setCamera(Camera *c) {
	camera = c;
	updateProjectionView();
}

void Graphics::updateProjectionView() {
	if (camera) {
		projectionView = projection * camera->view;
	}
}

void Graphics::beginScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->update();
}

void Graphics::set3d() {
	shader3d.setActive();
	glEnable(GL_DEPTH_TEST);
}

void Graphics::render3d(const Matrix4x4 &modelTransform, VertexArray *vao, TextureHandle tex) {
	Matrix4x4 transform = projectionView * modelTransform;

	shaderMVP.set(transform);

	//Bind our vertex array object and texture
	glBindVertexArray(decodeHandle(vao->vao));
	glBindTexture(GL_TEXTURE_2D, decodeHandle(tex));

	//FIXME: Integrate this properly to pick this up
	if (vao->ibuff != 0) {
		glDrawElements(GL_TRIANGLES, vao->primitiveCount * 3, GL_UNSIGNED_INT, nullptr);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vao->vertexCount);
	}
}

void Graphics::draw3d() {
	for (auto mi = modelRenderBuffer.begin(), me = modelRenderBuffer.end(); mi != me; ++mi) {
		Model *model = mi->first;
		std::vector<Matrix4x4*> &val = mi->second;

		for (int i = 0; i < val.size(); i++) {
			Matrix4x4 &modelTransform = *val[i];

			render3d(modelTransform, model->buffers, model->texture->getHandle());
		}
	}

	if (drawDebug && debugDrawer != NULL) {
		shader3dcolor.setActive();

		//Send in world transform
		shaderMVP.set(projectionView);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint buffers[2];
		glGenBuffers(2, buffers);

		//Create our line buffer in the GPU
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debugDrawer->linesBuffer.size(), debugDrawer->linesBuffer.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debugDrawer->colorBuffer.size(), debugDrawer->colorBuffer.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//Draw the lines
		glDrawArrays(GL_LINES, 0, debugDrawer->linesBuffer.size() / 3);

		//Now that we've drawn it, delete and clear the buffers
		glDeleteBuffers(2, buffers);
		glDeleteVertexArrays(1, &vao);

		debugDrawer->linesBuffer.clear();
		debugDrawer->colorBuffer.clear();
	}
}

void Graphics::set2d() {
	shader2d.setActive();
	glDisable(GL_DEPTH_TEST);
}

void Graphics::draw2d() {
	//TODO: Potential threading issues, need to make our own vector sometime
	for (int i = 0; i < spriteRenderBuffer.size(); i++) {
		Sprite *sprite = spriteRenderBuffer[i];
		
		render2d(sprite->vertices, sprite->texture->getHandle(), sprite->position);
	}

	//Clear the sprite buffer
	spriteRenderBuffer.clear();
}

void Graphics::render2d(VertexArray *vertices, TextureHandle texture, Vector2 &position) {
	//Set shader variables
	shader2dAspectRatio.set(aspectRatio);
	shader2dSpritePos.set(position);
	
	//Bind resources
	glBindVertexArray(decodeHandle(vertices->vao));
	glBindTexture(GL_TEXTURE_2D, decodeHandle(texture));

	//Draw.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices->vertexCount);
}

void Graphics::endScene() {
	//Buffer swap
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Graphics::clearModelBuffer() {
	modelRenderBuffer.clear();
}

void Graphics::addSprite(Sprite *sprite) {
	spriteRenderBuffer.push_back(sprite);
}

void Graphics::addModelInstance(ModelInstance *modelInstance) {
	if (modelInstance == NULL)
		return;
	auto mi = modelRenderBuffer.find(modelInstance->model);
	if (mi == modelRenderBuffer.end()) {
		//Inner model not in the map, so add it
		mi = modelRenderBuffer.insert(std::make_pair(modelInstance->model, std::vector<Matrix4x4*>())).first;
	}
	std::vector<Matrix4x4*> &val = mi->second;
	val.push_back(modelInstance->transform);
}

GLFWwindow *Graphics::getWindowHandle() {
	return window;
}

