#include "RigidBodySystem.h"

RigidBodySystem::RigidBodySystem(PxScene* gScene, PxPhysics* gPhysics)
{
	_gScene = gScene;
	_gPhysics = gPhysics;
	max_bodies = 50;
}

RigidBodySystem::~RigidBodySystem()
{
}

void RigidBodySystem::update(double t)
{
	if (_rigidBodies.size() < max_bodies) {
		addRigidBody(PxTransform(PxVec3(0, 20, 0)), Vector3(10, 0, 0), Vector3(10, 10, 10), Vector4(0.5, 0.5, 0.5, 1.0), 2, 100, 0);
	}
	
}
void RigidBodySystem::addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes)
{
	RenderItem* rI;
	PxShape* shape;


	PxRigidDynamic* new_solid;
	new_solid = _gPhysics->createRigidDynamic(PxTransform(tr));
	new_solid->setLinearVelocity(vel);
	new_solid->setAngularVelocity({ 1.0,0,0 });
	shape = CreateShape(PxBoxGeometry(size / 2.0));
	new_solid->attachShape(*shape);
	Vector3 inertia = { size.y * size.y + size.z * size.z,
					   size.x * size.x + size.z * size.z,
					   size.y * size.y + size.x * size.x };
	new_solid->setMass(mass);
	new_solid->setMassSpaceInertiaTensor(inertia * new_solid->getMass() / 12.0);
	rI = new RenderItem(shape, new_solid, color);
	_gScene->addActor(*new_solid);
};
