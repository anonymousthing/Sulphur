#pragma once

class Terrain;

class TerrainLoader {
public:
	static Terrain *loadTerrain(const char *terrainFile);
};
