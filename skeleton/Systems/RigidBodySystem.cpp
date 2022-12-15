#include "RigidBodySystem.h"

RigidBodySystem::RigidBodySystem(PxScene* gScene, PxPhysics* gPhysics)
{
	_gScene = gScene;
	_gPhysics = gPhysics;
	_max_bodies = 50;
	_spawn_delay = glutGet(GLUT_ELAPSED_TIME) + 1000;

	//add static objects
	//suelo
	addRigidBody(PxTransform({ 0.0f, 0.0f, 0.0f }), Vector3(), { 1000, 0.1, 1000 }, { 0.2,0.8,0.8,1.0 }, 1.0, -1, -1, true);
	//paredes
	addRigidBody(PxTransform({ 10,10,-300 }), Vector3(), { 400, 200, 5 }, { 0.8,0.8,0.8,1.0 }, 1.0, -1, -1, true);

	_registry = new RigidBodyForceRegistry();
	_wind = new WindForceGeneratorRB(0.9, 0.1, { 0.0,0.0,-100.0 }, 200, { 0.0,50.0,0.0 });
}

RigidBodySystem::~RigidBodySystem()
{
	for (auto rb : _rigidBodies)
		delete rb;

	_rigidBodies.clear();

	delete _registry;
	delete _wind;
}

void RigidBodySystem::update(double t)
{

	if (_rigidBodies.size() < _max_bodies && glutGet(GLUT_ELAPSED_TIME) >= _spawn_delay) {
		addRigidBody(PxTransform(PxVec3(0, 50, 0)), Vector3(10, 30, 0), Vector3(10, 10, 10), randomColor(), 2, 6000, 2000, false);
		_spawn_delay = glutGet(GLUT_ELAPSED_TIME) + 1000;
	}

	_registry->updateForces(t);

	//update de las particulas
	std::list<Rigidbody*>::iterator it = _rigidBodies.begin();
	while (it != _rigidBodies.end()) {
		Rigidbody* p = *it;
		if (!p)continue;
		if (p->isAlive()) {
			p->integrate(t);
			++it;
		}
		else {
			_registry->deleteBodyRegistry(p);
			delete p;
			it = _rigidBodies.erase(it);
		}
	}
}
void RigidBodySystem::addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, bool isStatic)
{
	Rigidbody* rb = new Rigidbody(tr, vel, size, color, mass, life, posDes, _gScene, _gPhysics, isStatic);
	_rigidBodies.push_back(rb);
	if (!isStatic)
		_registry->addRegistry(_wind, rb);
};

Vector4 RigidBodySystem::randomColor()
{
	return Vector4((rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, 1.0);
}