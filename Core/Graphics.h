#pragma once

#include <glew\glew.h>
#include <glfw\glfw3.h>

#include "DebugGraphics.h"
#include "ModelInstance.h"
#include "Sprite.h"
#include "GraphicsHandles.h"
#include "Vector.h"
#include "Camera.h"

#include "Shader.h"

#include <map>
#include <vector>

class Graphics {
    //This buffer is NOT cleared per frame, as we only expect 1 3D scene to be running at a time
    //You must remove ModelInstances from this manually
	std::map<Model*, std::vector<Matrix4x4*>> modelRenderBuffer;
    
    //This buffer is cleared every frame. You must add sprites to this buffer every draw call or you will
    //only see your sprite for one frame.
    std::vector<Sprite*> spriteRenderBuffer;

	Shader shader3d;
	Shader shader3dcolor;
	Shader shader2d;

	ShaderParameter shaderSampler;
	ShaderParameter shaderMVP;
	ShaderParameter shader2dAspectRatio;
	ShaderParameter shader2dSpritePos;

	float aspectRatio;
	GLFWwindow *window;
	int width = 800;
	int height = 600;
	float pixelWidth;
	float pixelHeight;

	Matrix4x4 worldTransform;
	Matrix4x4 projectionView;
	Matrix4x4 projection;

	void initProjection();
	void initGL();
public:
	DebugGraphics *debugDrawer = NULL;
	bool drawDebug = false;

	void setResolution(int width, int height);
	float getPixelWidth();
	float getPixelHeight();

	Camera *camera = nullptr;
	void setCamera(Camera *c);
	void updateProjectionView();

	Graphics();
	~Graphics();
	
    void beginScene();

    void set3d();
    void draw3d();
	void render3d(const Matrix4x4 &transform, VertexArray *vao, TextureHandle tex);
	
    void set2d();
    void draw2d();
	void render2d(VertexArray *vao, TextureHandle texture, Vector2 &position);

	void endScene();

    void addSprite(Sprite *sprite);
    void addModelInstance(ModelInstance *instance);

	void clearModelBuffer();

	GLFWwindow *getWindowHandle();

	void testScriptMovement();
};
