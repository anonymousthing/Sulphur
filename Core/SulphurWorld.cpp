#include "SulphurWorld.h"
#include "Log.h"

static Log logger("SulphurWorld");

SulphurWorld::SulphurWorld(Graphics *graphics) : graphics(graphics) {
	physics = new Physics();

	DebugGraphics *debugDrawer = new DebugGraphics();
	debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	physics->setDebugDrawer(debugDrawer);

	graphics->debugDrawer = debugDrawer;

	addGround();
}

void SulphurWorld::addGround() {
	//Plane is 0x + 1y + 0z = 1
	//Make the shapes (collision areas) for the ball and ground
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

	//Set the motion states of them
	//http://www.bulletphysics.org/mediawiki-1.5.8/index.php/MotionStates for more information
	//Read up on this!
	//We don't want anything special (just need to supply orientation and position), so just use Bullet's default motion state for now
	//Positioned at Y = -1
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	//Construct the actual rigid bodies for them

	//Ground
	//First argument is mass, last is inertia - both are zero so ground is "immovable". Mass of 0 in Bullet is infinite mass.
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundBody = new btRigidBody(groundRigidBodyCI);

	//Actually add the ground to the world
	SulphurEntity *groundEntity = new SulphurEntity(NULL, groundBody);
	groundEntity->description = "Ground";
	//Have to add ground manually as we don't want to add it to graphics (we have a NULL modelInstance)
	entities.push_back(groundEntity);
	physics->addRigidBody(groundEntity->body);
}

void SulphurWorld::bindCamera(SulphurEntity *entity) {
	cameraEntity = entity;
}

void SulphurWorld::setCameraTarget(Vector3 target) {
	Matrix4x4 &cameraEntityTrans = (*cameraEntity->modelInstance->transform);
	Vector3 cameraEntityPos(cameraEntityTrans[3][0], cameraEntityTrans[3][1], cameraEntityTrans[3][2]);
	cameraLookDirection = target - cameraEntityPos;
}

void SulphurWorld::addEntity(SulphurEntity *entity) {
	entities.push_back(entity);
	physics->addRigidBody(entity->body);
	graphics->addModelInstance(entity->modelInstance);
}

SulphurWorld::~SulphurWorld() {
	delete physics;
	graphics->clearModelBuffer();

	std::vector<Model*> baseModels;

	for (int i = 0; i < entities.size(); i++) {
		//If the entity has a model, add it to the list to be destroyed
		if (entities[i]->modelInstance != NULL) {
			Model *model = entities[i]->modelInstance->model;
			if (std::find(baseModels.begin(), baseModels.end(), model) == baseModels.end())
				baseModels.push_back(model);
		}
		delete entities[i];
	}

	for (int i = 0; i < baseModels.size(); i++) {
		delete baseModels[i];
	}
}

void SulphurWorld::updateCamera() {
	Matrix4x4 &cameraEntityTrans = (*cameraEntity->modelInstance->transform);
	Vector3 cameraEntityPos(cameraEntityTrans[3][0], cameraEntityTrans[3][1], cameraEntityTrans[3][2]);

	graphics->camera->setPosition(cameraEntityPos + cameraOffset);
	graphics->camera->setLookTarget(cameraEntityPos + cameraLookDirection);
}

void SulphurWorld::tick(int delta) {
	physics->tick(delta);

	//Our physics rigidBodies are updated, but now update the graphical model
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i] == cameraEntity) {
			int i = 0;
		}
		if (entities[i]->body->isActive()){
			btTransform trans;
			entities[i]->body->getMotionState()->getWorldTransform(trans);

			trans.getOpenGLMatrix(entities[i]->modelInstance->transform->data);
		}
	}

	for (auto hi = handlers.begin(), he = handlers.end(); hi != he; ++hi) {
		TickHandler handler = *hi;

		handler(delta);
	}

	updateCamera();
}

void SulphurWorld::resetPhysics() {
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->body->activate();
		entities[i]->body->setLinearVelocity(btVector3(0, 10, 0));
	}
}
