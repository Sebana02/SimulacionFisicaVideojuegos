#include "Rigidbody.h"

Rigidbody::Rigidbody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, PxScene* gScene, PxPhysics* gPhysics, bool is_static)
{
	_duration = life;
	(life > 0 ? _lifeTime = glutGet(GLUT_ELAPSED_TIME) + _duration : _lifeTime = -1);
	(posDes > 0 ? _lifePos = abs(tr.p.magnitude()) + posDes : _lifePos = -1);

	_gScene = gScene;
	_gPhysics = gPhysics;

	_static = is_static;
	
	if (_static) {
		_solid = _gPhysics->createRigidStatic(PxTransform(tr));
	}
	else {
		_solid = _gPhysics->createRigidDynamic(PxTransform(tr));
		PxRigidDynamic* new_solid = static_cast<physx::PxRigidDynamic*>(_solid);
		new_solid->setLinearVelocity(vel);
		new_solid->setAngularVelocity({ 1.0,0,0 });



		Vector3 inertia = { size.y * size.y + size.z * size.z,
						   size.x * size.x + size.z * size.z,
						   size.y * size.y + size.x * size.x };
		new_solid->setMass(mass);
		new_solid->setMassSpaceInertiaTensor(inertia * new_solid->getMass() / 12.0);

		
	}
	
	
	//render
	PxShape* shape = CreateShape(PxBoxGeometry(size / 2.0));
	_solid->attachShape(*shape);
	_renderItem = new RenderItem(shape, _solid, color);
	_gScene->addActor(*_solid);


	_alive = true;
}
Rigidbody::~Rigidbody()
{
	DeregisterRenderItem(_renderItem);
	delete _renderItem;
	_gScene->removeActor(*_solid);
}

void Rigidbody::integrate(double t)
{
	if ((_lifeTime > 0 && glutGet(GLUT_ELAPSED_TIME) >= _lifeTime) ||
		(_lifePos > 0 && (abs(_solid->getGlobalPose().p.magnitude()) > _lifePos)))
		_alive = false;
}
