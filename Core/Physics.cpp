#include "Physics.h"

#include "SulphurEntity.h"

#include <stdio.h>

btVector3 gravity(0.0f, -9.81f, 0.0f);

Physics::Physics() {
	initPhysics();
}

Physics::~Physics() {
	delete world;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}

bool onContactProcess(btManifoldPoint &cp, void *_bodyA, void *_bodyB);

int Physics::initPhysics() {
	printf("Initializing Bullet...\n");

	//SETUP BEGINS HERE
	//Specifies the broadphase algorithm. Find out what Dbvt stands for!
	broadphase = new btDbvtBroadphase();
	//Specifies the collision configuration. Using the default BulletPhysics one for now - look more into this
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//Dispatches collisions to a callback
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	//Solver stuff - this literally "solves" the physics such as gravity, forces, collisions and constraints, etc.
	//Does the magic.
	solver = new btSequentialImpulseConstraintSolver();

    gContactProcessedCallback = (ContactProcessedCallback)&onContactProcess;

	//WORLD STUFF BEGINS HERE
	//Make the world, and pass it all of our magic physics stuff we initialised before
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	//Set gravity to downwards, in m/s^2
	world->setGravity(gravity);

	return 0;
}

void Physics::setDebugDrawer(btIDebugDraw *drawer) {
    world->setDebugDrawer(drawer);
}

void Physics::addRigidBody(btRigidBody *body) {
	world->addRigidBody(body);
    
    
}

btRigidBody *Physics::newBoxBody(btVector3 position) {
	//Box
	btCollisionShape* boxShape = new btBoxShape(btVector3(1.f, 1.f, 1.f)); //1x1x1 cube
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));

    //Give the box mass of 10000kg

	btScalar boxMass = 10000;
	btVector3 boxInertia(0, 0, 0);
	boxShape->calculateLocalInertia(boxMass, boxInertia);

	btRigidBody::btRigidBodyConstructionInfo boxInfo(boxMass, boxMotionState, boxShape, boxInertia);
	boxInfo.m_restitution = 0.6f;
	boxInfo.m_friction = 1.5f;

	return new btRigidBody(boxInfo);
}

float stepRate = 250;

void Physics::tick(int delta) {
	world->stepSimulation(delta / 1000.f, 1, 1 / 20.f);
	if (drawDebug)
		world->debugDrawWorld();

    //int numManifolds = world->getDispatcher()->getNumManifolds();
    //for (int i = 0; i < numManifolds; i++) {
    //    btPersistentManifold *manifold = world->getDispatcher()->getManifoldByIndexInternal(i);
    //    const btCollisionObject *object1 = manifold->getBody0();
    //    const btCollisionObject *object2 = manifold->getBody1();

    //    int numContacts = manifold->getNumContacts();
    //    for (int i = 0; i < numContacts; i++) {
    //        btManifoldPoint &pt = manifold->getContactPoint(i);
    //        if (pt.getDistance() < 0.f) {
    //            Entity *entity1 = (Entity*)object1->getUserPointer();
    //            Entity *entity2 = (Entity*)object2->getUserPointer();
    //            if (strcmp(entity1->description, "Ground") != 0 && strcmp(entity2->description, "Ground") != 0)
    //                printf("Collided!\n");
    //        }
    //    }
    //}
}

bool onContactProcess(btManifoldPoint &cp, void *_bodyA, void *_bodyB) {
    btRigidBody *bodyA = (btRigidBody*)_bodyA;
    btRigidBody *bodyB = (btRigidBody*)_bodyB;

	SulphurEntity *entityA = (SulphurEntity*)bodyA->getUserPointer();
	SulphurEntity *entityB = (SulphurEntity*)bodyB->getUserPointer();
    
    if (strcmp(entityA->description, "Ground") != 0 && strcmp(entityB->description, "Ground") != 0)
        printf("Collided!\n");

    return true;
}
