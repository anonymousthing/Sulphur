#pragma once

#include "Vector.h"
#include "GraphicsHandles.h"
#include "VertexArray.h"

class Graphics;

class Terrain {
private:
	//TODO: Actual quad-tree based LOD terrain
	
	TextureHandle grass;

	//TODO: This needs to be removed eventually
	VertexArray *vertices;

public:
	Terrain(TextureHandle grass, VertexArray *vertices);
	~Terrain();

	//Uses actorPosition to reduce detail for and later unload distant chunks.
	void update(const Vector3 &actorPosition);
	void draw(Graphics *graphics);


};
