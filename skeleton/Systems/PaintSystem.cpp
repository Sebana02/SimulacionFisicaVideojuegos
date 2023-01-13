#include "PaintSystem.h"
#include <iostream>

PaintSystem::PaintSystem(PxScene* gScene, PxPhysics* gPhysics)
{
	_gScene = gScene;
	_gPhysics = gPhysics;


	//canvas
	addRigidBody(PxTransform({ 10,0,0 }), Vector3(), { 10,10,20 }, { 255,255,255,255 }, 1.0, -1, -1, true, false);

	//pincel
	pincel = new GaussianRBGenerator(GetCamera()->getTransform().p - GetCamera()->getDir(), GetCamera()->getDir() * 10, { 1,1,1 }, Vector3(), 100, 1);
	pincel->setParticle(new Rigidbody(GetCamera()->getTransform(), Vector3(), { .1,.1,.1 }, { 0,0,0,255 }, 1.0, -1, -1, gScene, gPhysics, false, true));

	_registry = new RigidBodyForceRegistry();

}

PaintSystem::~PaintSystem()
{
	for (auto rb : _rigidBodies)
		delete rb;

	_rigidBodies.clear();

	delete pincel;

	delete _registry;
}

void PaintSystem::update(double t)
{
	_registry->updateForces(t);

	if (paint) {
		pincel->setVelocity(GetCamera()->getDir() * 10);
		pincel->setOrigin(GetCamera()->getTransform().p - GetCamera()->getDir());

		std::list<Rigidbody*> bodies = pincel->generateParticles();
		_rigidBodies.insert(_rigidBodies.end(), bodies.begin(), bodies.end());
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
			_registry->deleteBodyRegistry(p);
			it = _rigidBodies.erase(it);
			delete p;
		}
	}
}
Rigidbody* PaintSystem::addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, bool isStatic, bool sphere)
{
	Rigidbody* rb = new Rigidbody(tr, vel, size, color, mass, life, posDes, _gScene, _gPhysics, isStatic, sphere);
	_rigidBodies.push_back(rb);

	return rb;
};

Vector4 PaintSystem::randomColor()
{
	return Vector4((rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, 1.0);
}