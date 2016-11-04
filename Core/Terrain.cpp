#include "Terrain.h"

Terrain::Terrain(TextureHandle grass, VertexArray *vertices)
	: grass(grass), vertices(vertices) {

}

Terrain::~Terrain() {
	releaseHandle(grass);
	delete vertices;
}

void Terrain::update(const Vector3 &actorPosition) {

}

void Terrain::draw(Graphics *graphics) {
	
}
