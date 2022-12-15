#include "Rigidbody.h"

Rigidbody::Rigidbody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, PxScene* gScene, PxPhysics* gPhysics)
{
	_duration = life;
	(life > 0 ? _lifeTime = glutGet(GLUT_ELAPSED_TIME) + _duration : _lifeTime = -1);
	(posDes > 0 ? _lifePos = abs(tr.p.magnitude()) + posDes : _lifePos = -1);
	
	_gScene = gScene;
	_gPhysics = gPhysics;

	solid = _gPhysics->createRigidDynamic(PxTransform(tr));
	solid->setLinearVelocity(vel);
	solid->setAngularVelocity({ 1.0,0,0 });

	shape = CreateShape(PxBoxGeometry(size / 2.0));
	solid->attachShape(*shape);

	Vector3 inertia = { size.y * size.y + size.z * size.z,
					   size.x * size.x + size.z * size.z,
					   size.y * size.y + size.x * size.x };
	solid->setMass(mass);
	solid->setMassSpaceInertiaTensor(inertia * solid->getMass() / 12.0);

	_renderItem = new RenderItem(shape, solid, color);
	_gScene->addActor(*solid);
	RegisterRenderItem(_renderItem);

	_alive = true;
}
Rigidbody::~Rigidbody()
{
	DeregisterRenderItem(_renderItem);
	delete _renderItem;
	solid->detachShape(*shape);
	_gScene->removeActor(*solid);
}

void Rigidbody::integrate(double t)
{
	if ((_lifeTime > 0 && glutGet(GLUT_ELAPSED_TIME) >= _lifeTime) ||
		(_lifePos > 0 && (abs(solid->getGlobalPose().p.magnitude()) > _lifePos)))
		_alive = false;
}
