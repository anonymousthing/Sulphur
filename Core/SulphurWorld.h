#pragma once

#include "SulphurEntity.h"
#include "Physics.h"
#include "Graphics.h"
#include "DebugGraphics.h"

#include <functional>

//TODO: TickHandler should take World
class SulphurWorld;
typedef std::function<void(int)> TickHandler;

class SulphurWorld {
	Graphics *graphics;

	std::vector<TickHandler> handlers;
	void addGround();
public:
	Physics *physics;
	std::vector<SulphurEntity*> entities;

	SulphurEntity *cameraEntity;
	Vector3 cameraOffset;
	Vector3 cameraLookDirection;

	float movespeed = 10.0f;

	SulphurWorld(Graphics *graphics);
	~SulphurWorld();
	void draw();
	
	void updateCamera();
	void tick(int step);
	void resetPhysics();

	void bindCamera(SulphurEntity *entity);
	void setCameraTarget(Vector3 target);
	void addEntity(SulphurEntity *entity);

	void registerTickHandler(TickHandler handler) {
		handlers.push_back(handler);
	}
};
