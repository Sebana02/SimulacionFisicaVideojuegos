#include "RigidBodySystem.h"

RigidBodySystem::RigidBodySystem(PxScene* gScene, PxPhysics* gPhysics)
{
	_gScene = gScene;
	_gPhysics = gPhysics;
	_max_bodies = 50;
	_spawn_delay = glutGet(GLUT_ELAPSED_TIME) + 1000;
}

RigidBodySystem::~RigidBodySystem()
{
	for (auto rb : _rigidBodies) 
		delete rb;

	_rigidBodies.clear();
}

void RigidBodySystem::update(double t)
{
	
	if (_rigidBodies.size() < _max_bodies && glutGet(GLUT_ELAPSED_TIME) >= _spawn_delay) {
		addRigidBody(PxTransform(PxVec3(0, 50, 0)), Vector3(10, 0, 0), Vector3(10, 10, 10), Vector4(0.5, 0.5, 0.5, 1.0), 2, 2000, 2000);
		_spawn_delay = glutGet(GLUT_ELAPSED_TIME) + 1000;
	}
	
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
			delete p;
			it = _rigidBodies.erase(it);
		}
	}
}
void RigidBodySystem::addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes)
{
	_rigidBodies.push_back(new Rigidbody(tr, vel, size, color, mass, life, posDes, _gScene, _gPhysics));
};
