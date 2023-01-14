#include "PaintSystem.h"
#include <iostream>

PaintSystem::PaintSystem(PxScene* gScene, PxPhysics* gPhysics)
{
	_gScene = gScene;
	_gPhysics = gPhysics;

	_speed = 10;

	//canvas
	canvas = addRigidBody(PxTransform({ 10,0,0 }), Vector3(), { 1,5,10 }, { 255,255,255,255 }, 1.0, -1, -1, true, false, Rigidbody::CANVAS);

	//pincel
	pincel = new GaussianRBGenerator(GetCamera()->getTransform().p - GetCamera()->getDir(), GetCamera()->getDir() * _speed, { .5,.5,.5 }, Vector3(), 100, 1);
	pincel->setParticle(new Rigidbody(GetCamera()->getTransform(), Vector3(), { .1,.1,.1 }, { .5,.5,.2,255 }, 1.0, -1, 25, gScene, gPhysics, false, true, Rigidbody::ARRIVING_PAINT));


	//borrador
	borrador = new GaussianRBGenerator(GetCamera()->getTransform().p - GetCamera()->getDir(), GetCamera()->getDir() * _speed, { 1,1,1 }, Vector3(), 10, 1);
	borrador->setParticle(new Rigidbody(GetCamera()->getTransform(), Vector3(), { .1,.1,.1 }, { .5,.5,.5,.2 }, 1.0, -1, 25, gScene, gPhysics, false, true, Rigidbody::ERASING_PAINT));

	//clear
	clear = new WindForceGeneratorRB(1, 0, { 0.0,-20.0,0.0 }, 50, { 10,0,0 });
	clear->setActive(true);

	
	_registry = new RigidBodyForceRegistry();

}

PaintSystem::~PaintSystem()
{
	for (auto rb : _rigidBodies)
		delete rb;

	_rigidBodies.clear();

	delete pincel;
	delete borrador;
	delete clear;

	delete _registry;
}

void PaintSystem::update(double t)
{
	//cout << _rigidBodies.size() << endl;


	if (paint) { //pintar
		pincel->setVelocity(GetCamera()->getDir() * _speed);
		pincel->setOrigin(GetCamera()->getTransform().p - GetCamera()->getDir());

		std::list<Rigidbody*> bodies = pincel->generateParticles();
		_rigidBodies.insert(_rigidBodies.end(), bodies.begin(), bodies.end());
	}
	else if (eraser) {
		borrador->setVelocity(GetCamera()->getDir() * _speed);
		borrador->setOrigin(GetCamera()->getTransform().p - GetCamera()->getDir());

		std::list<Rigidbody*> bodies = borrador->generateParticles();
		_rigidBodies.insert(_rigidBodies.end(), bodies.begin(), bodies.end());
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
			it = _rigidBodies.erase(it);
			delete p;
		}
	}
}
Rigidbody* PaintSystem::addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, bool isStatic, bool sphere, Rigidbody::type t)
{
	Rigidbody* rb = new Rigidbody(tr, vel, size, color, mass, life, posDes, _gScene, _gPhysics, isStatic, sphere, t);
	_rigidBodies.push_back(rb);

	return rb;
};

Vector4 PaintSystem::randomColor()
{
	return Vector4((rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, 1.0);
}

void PaintSystem::clearCanvas()
{
	for (auto& body : _rigidBodies) {
		if (body->isAlive() && body->_type != Rigidbody::CANVAS) {
			static_cast<physx::PxRigidDynamic*>(body->getActor())->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			body->_type = Rigidbody::LEAVING_PAINT;
			_registry->addRegistry(clear, body);
		}
	}	
}

void PaintSystem::changeColor(int n)
{
	Vector4 color;

	switch (n) {
	case 0:
		color = { 0,0,0,1 };
		break;
	case 1:
		color = { 1,0,0, 1 };
		break;
	case 2:
		color = { 0,1,0, 1 };
		break;
	case 3:
		color = { 0,0,1, 1 };
		break;
	case 4:
		color = { 1,1,0, 1 };
		break;
	case 5:
		color = { 1,0,1, 1 };
		break;
	case 6:
		color = { 0,1,1, 1 };
		break;
	case 7:
		color = { 1,1,1, 1 };
		break;
	case 8:
		color = { 0.5,0.5,0.5, 1 };
		break;
	case 9:
		color = { 0.5,0,0, 1 };
		break;
	default:
		break;
	}

	pincel->setParticle(new Rigidbody(GetCamera()->getTransform(), Vector3(), { .1,.1,.1 }, color, 1.0, -1, -1, _gScene, _gPhysics, false, true, Rigidbody::ARRIVING_PAINT));
}
