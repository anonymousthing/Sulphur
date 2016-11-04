#include "SulphurEntity.h"

SulphurEntity::SulphurEntity(ModelInstance *innerModel, btRigidBody *innerBody, const char *description)
     : modelInstance(innerModel), body(innerBody), description(description){

    body->setUserPointer(this);
}

SulphurEntity::~SulphurEntity() {
	delete modelInstance;
	delete body;
}

void SulphurEntity::moveX(float xVel) {
	btVector3 currentVelocity = body->getLinearVelocity();
	body->setLinearVelocity(btVector3(xVel, currentVelocity.getY(), currentVelocity.getZ()));
	body->activate();
}

void SulphurEntity::moveY(float yVel) {
	btVector3 currentVelocity = body->getLinearVelocity();
	body->setLinearVelocity(btVector3(currentVelocity.getX(), yVel, currentVelocity.getZ()));
	body->activate();
}

void SulphurEntity::moveZ(float zVel) {
	btVector3 currentVelocity = body->getLinearVelocity();
	body->setLinearVelocity(btVector3(currentVelocity.getX(), currentVelocity.getY(), zVel));
	body->activate();
}

void SulphurEntity::onCollision(SulphurEntity *other) {

}

SulphurEntity *SulphurEntity::getWorld() {
    return NULL;
}
