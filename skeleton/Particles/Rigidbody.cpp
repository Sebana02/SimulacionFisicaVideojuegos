#include "Rigidbody.h"
#include <iostream>

Rigidbody* Rigidbody::clone()
{

	return new Rigidbody(_solid->getGlobalPose(), static_cast<physx::PxRigidDynamic*>(_solid)->getLinearVelocity(),
		_size, _renderItem->color, static_cast<physx::PxRigidDynamic*>(_solid)->getMass(), _duration, _posDes,
		_gScene, _gPhysics, _static,_sphere);

}

void Rigidbody::onCollision(type t)
{
	_type = t;
}

Rigidbody::Rigidbody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, PxScene* gScene, PxPhysics* gPhysics, bool is_static,bool sphere)
{
	_duration = life;
	_posDes = posDes;

	(life > 0 ? _lifeTime = glutGet(GLUT_ELAPSED_TIME) + _duration : _lifeTime = -1);
	(posDes > 0 ? _lifePos = abs(tr.p.magnitude()) + posDes : _lifePos = -1);

	_gScene = gScene;
	_gPhysics = gPhysics;

	_static = is_static;

	if (_static) {
		_solid = _gPhysics->createRigidStatic(PxTransform(tr));
		_type = CANVAS;
	}
	else {
		_solid = _gPhysics->createRigidDynamic(PxTransform(tr));
		PxRigidDynamic* new_solid = static_cast<physx::PxRigidDynamic*>(_solid);
		new_solid->setLinearVelocity(vel);


		Vector3 inertia = { size.y * size.y + size.z * size.z,
						   size.x * size.x + size.z * size.z,
						   size.y * size.y + size.x * size.x };
		new_solid->setMass(mass);
		//new_solid->setMassSpaceInertiaTensor(inertia * new_solid->getMass() / 12.0);
		_type = ARRIVING_PAINT;
	}

	//render
	_size = size;
	
	if(sphere)
		_shape = CreateShape(PxSphereGeometry(size.x / 2.0));
	else
		_shape = CreateShape(PxBoxGeometry(size / 2.0));
	
	_renderItem = new RenderItem(_shape, _solid, color);
	_solid->attachShape(*_shape);
	_gScene->addActor(*_solid);

	_alive = true;


	_solid->userData = this;

}
Rigidbody::~Rigidbody()
{
	DeregisterRenderItem(_renderItem);
	delete _renderItem;
	_gScene->removeActor(*_solid);
}

void Rigidbody::integrate(double t)
{
	if (_type == TO_DELETE || (_lifeTime > 0 && glutGet(GLUT_ELAPSED_TIME) >= _lifeTime) ||
		(_lifePos > 0 && (abs(_solid->getGlobalPose().p.magnitude()) > _lifePos)))
		_alive = false;

	if (_type == TO_STOP) {
		PxRigidDynamic* new_solid = static_cast<physx::PxRigidDynamic*>(_solid);
		new_solid->setLinearVelocity(Vector3());
		new_solid->setAngularVelocity(Vector3());
		new_solid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

		_type = STATIC_PAINT;
	}
}

