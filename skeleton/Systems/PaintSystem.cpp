#include "PaintSystem.h"
#include <iostream>

PaintSystem::PaintSystem(PxScene* gScene, PxPhysics* gPhysics)
{
	_gScene = gScene;
	_gPhysics = gPhysics;

	_speed = 10;
	_size = { 0.1,0.1,0.1 };
	_lifePos = 25;

	//canvas
	canvas = new Rigidbody(PxTransform({ 10,0,0 }), {0.0,0.0,0.0}, { 1,5,10 }, { 1,1,1,1 }, 1.0, -1, -1, gScene, gPhysics, true, false, Rigidbody::CANVAS);

	//pincel
	pincel = new GaussianRBGenerator(GetCamera()->getTransform().p - GetCamera()->getDir(), GetCamera()->getDir() * _speed, {0.0,0.0,0.0}, {0.0,0.0,0.0}, 1, 1);
	pincel->setParticle(new Rigidbody(GetCamera()->getTransform(), {0.0,0.0,0.0}, _size, { 0,0,0,1 }, 1.0, -1, _lifePos, gScene, gPhysics, false, true, Rigidbody::ARRIVING_PAINT));


	//borrador
	borrador = new GaussianRBGenerator(GetCamera()->getTransform().p - GetCamera()->getDir(), GetCamera()->getDir() * _speed, {0.0,0.0,0.0}, {0.0,0.0,0.0}, 1, 1);
	borrador->setParticle(new Rigidbody(GetCamera()->getTransform(), {0.0,0.0,0.0}, _size, { .5,.5,.5,.2 }, 1.0, -1, _lifePos, gScene, gPhysics, false, true, Rigidbody::ERASING_PAINT));


	//clear
	clear = new WindForceGeneratorRB(1, 0, { 0.0,0.0,0.0 }, 50, { 0,0,0 });
	clear->setActive(true);
	clear2 = new WhirlwindForceGeneratorRB(1, 0, 2, 200, { 5,0,0 });
	clear2->setActive(true);
	

	_registry = new RigidBodyForceRegistry();

}

PaintSystem::~PaintSystem()
{
	for (auto rb : _rigidBodies)
		delete rb;
	_rigidBodies.clear();
	delete canvas;

	delete pincel;
	delete borrador;
	delete clear;

	delete _registry;
}

void PaintSystem::update(double t)
{
	//cout << _rigidBodies.size() << endl;

	; //list of bodies to add to the scene
	if (paint) { //pintar
		pincel->setVelocity(GetCamera()->getDir() * _speed);
		pincel->setOrigin(GetCamera()->getTransform().p - GetCamera()->getDir());

		std::list<Rigidbody*> add_bodies = pincel->generateParticles();
		_rigidBodies.insert(_rigidBodies.end(), add_bodies.begin(), add_bodies.end());
	}
	else if (eraser) { //borrar
		borrador->setVelocity(GetCamera()->getDir() * _speed);
		borrador->setOrigin(GetCamera()->getTransform().p - GetCamera()->getDir());

		std::list<Rigidbody*> add_bodies = borrador->generateParticles();
		_rigidBodies.insert(_rigidBodies.end(), add_bodies.begin(), add_bodies.end());
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

Vector4 PaintSystem::randomColor()
{
	return Vector4((rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, 1.0);
}

void PaintSystem::clearCanvas()
{
	if (rand() % 2 == 0) { //clear con viento
		
		clear->setWind(Vector3(0, (rand() % 2 == 0 ? -1 : 1), (rand() % 2 == 0 ? -1 : 1)) * _speed);

		for (auto& body : _rigidBodies) {
			if (body->isAlive()) {
				static_cast<physx::PxRigidDynamic*>(body->getActor())->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
				body->_type = Rigidbody::LEAVING_PAINT;
				_registry->addRegistry(clear, body);
			}
		}
	}
	else { //clear con torbellino
		
		for (auto& body : _rigidBodies) {
			if (body->isAlive()) {
				static_cast<physx::PxRigidDynamic*>(body->getActor())->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
				body->_type = Rigidbody::LEAVING_PAINT;
				_registry->addRegistry(clear2, body);
			}
		}
	}
	
}

void PaintSystem::setThickness(bool thicker)
{
	Vector3 _anchura = pincel->getDevPos();
	
	if (thicker) {
		float anch = min(_anchura.x + 0.1, 1.0);
		_anchura = Vector3(anch,anch,anch);
	}
	else {
		float anch = max(_anchura.x - 0.1, 0.0);
		_anchura = Vector3(anch, anch, anch);
	}
	
	int numPar = _anchura.x * 10 + 10; // cada 0.1 de anchura son 10 particulas
	
	pincel->setDevPos(_anchura);
	borrador->setDevPos(_anchura);

	pincel->setParticlesNum(numPar);
	borrador->setParticlesNum(numPar);
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

	pincel->setParticle(new Rigidbody(GetCamera()->getTransform(), {0.0,0.0,0.0}, _size, color, 1.0, -1, _lifePos, _gScene, _gPhysics, false, true, Rigidbody::ARRIVING_PAINT));
}