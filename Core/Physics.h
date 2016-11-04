#pragma once

#include <vector>

#include "bullet\btBulletDynamicsCommon.h"

class Physics {
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;

    //void setRigidBodyPos(btRigidBody *body, btVector3 pos);
public:
	std::vector<btRigidBody*> bodies;

	Physics();
	~Physics();

	int initPhysics();
    void tick(int step);

	bool drawDebug = false;
    void setDebugDrawer(btIDebugDraw *drawer);


	void addRigidBody(btRigidBody *body);
	static btRigidBody *newBoxBody(btVector3 position);
};
