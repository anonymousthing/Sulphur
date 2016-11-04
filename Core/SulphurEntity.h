#pragma once

#include "bullet\btBulletDynamicsCommon.h"
#include "ModelInstance.h"

class SulphurEntity {
public:
    const char *description;
	ModelInstance *modelInstance;
	btRigidBody *body;


	void moveX(float xVel);
	void moveY(float yVel);
	void moveZ(float zVel);

    void onCollision(SulphurEntity *other);
    SulphurEntity *getWorld();

	SulphurEntity(ModelInstance *innerModel, btRigidBody *innerBody, const char *description = "");
	~SulphurEntity();
};
