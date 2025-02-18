#include "PaintSystem.h"
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

PaintSystem::PaintSystem(PxScene* gScene, PxPhysics* gPhysics)
{
	_gScene = gScene;
	_gPhysics = gPhysics;

	_speed = 10;
	_size = { 0.1,0.1,0.1 };
	_lifePos = 25;

	_paint = false;
	_eraser = false;

	_maxThickness = 0.5;

	_color = { 0,0,0,1 };
	//canvas
	_canvas = new Rigidbody(PxTransform({ 6,0,0 }), { 0.0,0.0,0.0 }, { 1,5,10 }, { 1,1,1,.1 }, 1.0, -1, -1, gScene, gPhysics, true, false, Rigidbody::CANVAS);

	//pincel
	_pincel = new GaussianRBGenerator(GetCamera()->getTransform().p - GetCamera()->getDir(), GetCamera()->getDir() * _speed, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 1, 1);
	_pincel->setParticle(new Rigidbody(GetCamera()->getTransform(), { 0.0,0.0,0.0 }, _size, { 0,0,0,1 }, 1.0, -1, _lifePos, gScene, gPhysics, false, true, Rigidbody::ARRIVING_PAINT));


	//borrador
	_borrador = new GaussianRBGenerator(GetCamera()->getTransform().p - GetCamera()->getDir(), GetCamera()->getDir() * _speed, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 1, 1);
	_borrador->setParticle(new Rigidbody(GetCamera()->getTransform(), { 0.0,0.0,0.0 }, _size, { .5,.5,.5,.2 }, 1.0, -1, _lifePos, gScene, gPhysics, false, true, Rigidbody::ERASING_PAINT));


	//clear
	_clear = new WindForceGeneratorRB(1, 0, { 0.0,0.0,0.0 }, 50, { 0,0,0 });
	_clear->setActive(true);
	_clear2 = new WhirlwindForceGeneratorRB(1, 0, 2, 200, { 1,0,0 });
	_clear2->setActive(true);


	//fuerzas
	_registry = new RigidBodyForceRegistry();


	//screenshot
	generateFireworksSystem();
	_photoDelay = 2000;
	_photoTimer = -1;
}

PaintSystem::~PaintSystem()
{
	for (auto rb : _rigidBodies)
		delete rb;
	_rigidBodies.clear();
	delete _canvas;

	delete _pincel;
	delete _borrador;
	delete _clear;
	delete _clear2;

	delete _registry;

	for (auto p : _particles)
		delete p;
	_particles.clear();

	for (auto f : _fireworks_pool)
		delete f;
	_fireworks_pool.clear();

	delete _firework_gen;
}

void PaintSystem::update(double t)
{
	//cout << _rigidBodies.size() << endl;
	//cout << _particles.size() << endl;

	//timer para la screenshot
	if (_photoTimer != -1 && glutGet(GLUT_ELAPSED_TIME) >= _photoTimer)
		takeScreenshot();




	if (_photoTimer == -1 && _paint) { //pintar
		_pincel->setVelocity(GetCamera()->getDir() * _speed);
		_pincel->setOrigin(GetCamera()->getTransform().p - GetCamera()->getDir());

		std::list<Rigidbody*> add_bodies = _pincel->generateParticles();
		_rigidBodies.insert(_rigidBodies.end(), add_bodies.begin(), add_bodies.end());
	}
	else if (_photoTimer == -1 && _eraser) { //borrar
		_borrador->setVelocity(GetCamera()->getDir() * _speed);
		_borrador->setOrigin(GetCamera()->getTransform().p - GetCamera()->getDir());

		std::list<Rigidbody*> add_bodies = _borrador->generateParticles();
		_rigidBodies.insert(_rigidBodies.end(), add_bodies.begin(), add_bodies.end());
	}


	_registry->updateForces(t);

	//update de los rigid bodies
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


	//update de las particulas
	std::list<Particle*>::iterator it2 = _particles.begin();
	while (it2 != _particles.end()) {
		Particle* p = *it2;
		if (!p)continue;
		if (p->isAlive()) {
			p->integrate(t);
			++it2;
		}
		else {
			if (p->_type == Particle::TYPE::FIREWORK) {
				Firework* firework = dynamic_cast<Firework*>(p);
				if (firework != nullptr) {
					std::list<Particle*> add_particles = firework->explode();
					_particles.insert(_particles.end(), add_particles.begin(), add_particles.end());
				}
			}
			delete p;
			it2 = _particles.erase(it2);
		}
	}

}

void PaintSystem::clearCanvas()
{
	_clear->setWind(Vector3(0, (rand() % 2 == 0 ? -1 : 1), (rand() % 2 == 0 ? -1 : 1)) * _speed);

	bool wind = (bool)(rand() % 2);

	for (auto& body : _rigidBodies) {
		if (body->isAlive()) {
			static_cast<physx::PxRigidDynamic*>(body->getActor())->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			body->_type = Rigidbody::LEAVING_PAINT;
			if (wind) //clear con viento
				_registry->addRegistry(_clear, body);
			else //clear con torbellino
				_registry->addRegistry(_clear2, body);

		}
	}
}

void PaintSystem::setThickness(bool thicker)
{
	Vector3 _anchura = _pincel->getDevPos();

	float anch;
	if (thicker)
		anch = min(_anchura.x + 0.05, _maxThickness);
	else
		anch = max(_anchura.x - 0.05, 0.0);


	_anchura = Vector3(anch, anch, anch);
	int numPar = _anchura.x * 20 + 1;

	_pincel->setDevPos(_anchura);
	_borrador->setDevPos(_anchura);

	_pincel->setParticlesNum(numPar);
	_borrador->setParticlesNum(numPar);
}

void PaintSystem::changeColor(int n)
{
	switch (n) {
	case 0:
		_color = { 0,0,0,1 };
		break;
	case 1:
		_color = { 1,0,0, 1 };
		break;
	case 2:
		_color = { 0,1,0, 1 };
		break;
	case 3:
		_color = { 0,0,1, 1 };
		break;
	case 4:
		_color = { 1,1,0, 1 };
		break;
	case 5:
		_color = { 1,0,1, 1 };
		break;
	case 6:
		_color = { 0,1,1, 1 };
		break;
	case 7:
		_color = { 1,1,1, 1 };
		break;
	case 8:
		_color = { 0.5,0.5,0.5, 1 };
		break;
	case 9:
		_color = { 0.5,0,0, 1 };
		break;
	default:
		break;
	}

	_pincel->setParticle(new Rigidbody(GetCamera()->getTransform(), { 0.0,0.0,0.0 }, _size, _color, 1.0, -1, _lifePos, _gScene, _gPhysics, false, true, Rigidbody::ARRIVING_PAINT));
}

void PaintSystem::prepareScreenshot() {

	if (_photoTimer != -1) return;

	_photoTimer = glutGet(GLUT_ELAPSED_TIME) + _photoDelay;

	//genera fireworks
	if (_fireworks_pool.size() > 0) {
		Vector3 pos = _canvas->getActor()->getGlobalPose().p;

		int num = 100;
		float aum = 360.0 / num;

		for (int i = 0; i < num; i++) {

			Vector3 new_pos = Vector3(pos.x + 60 * sin(aum * i), pos.y - 2 * _canvas->getSize().y, pos.z + 60 * cos(aum * i));

			_firework_gen->setOrigin(new_pos);
			_firework_gen->setParticle(_fireworks_pool.at(rand() % _fireworks_pool.size())->clone());

			for (auto& p : _firework_gen->generateParticles())
				_particles.push_back(p);
		}
	}
	
	
	//posiciona la camara e impide que se mueva
	GetCamera()->setLock(true);
	GetCamera()->setDir(Vector3(1.0, 0.3f, 0));
	GetCamera()->setEye(Vector3(-10.0f, 0.0f, 0.0f));

	_paint = false;
	_eraser = false;
}

void PaintSystem::takeScreenshot()
{
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	unsigned char* pixels = (unsigned char*)malloc(width * height * 3);

	// Read the pixels from the framebuffer
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// Flip the image vertically
	for (int y = 0; y < height / 2; y++) {
		for (int x = 0; x < width; x++) {
			for (int c = 0; c < 3; c++) {
				int top = (y * width + x) * 3 + c;
				int bottom = ((height - 1 - y) * width + x) * 3 + c;
				unsigned char temp = pixels[top];
				pixels[top] = pixels[bottom];
				pixels[bottom] = temp;
			}
		}
	}

	// Save the pixels to a file
	stbi_write_png("screenshot.png", width, height, 3, pixels, width * 3);

	// Free the allocated memory
	free(pixels);


	//desbloquea la camara
	_photoTimer = -1;
	GetCamera()->setEye(Vector3(0.0f, 0.0f, 0.0f));
	GetCamera()->setDir(Vector3(1.0, 0.0f, 0));
	GetCamera()->setLock(false);
}

void PaintSystem::generateFireworksSystem() {

	//circulo
	Vector4 randColor;
	float mass;

	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		mass = randomMass();
		shared_ptr<ParticleGenerator> g(new CircleParticleGenerator({ 0.0,0.0,0.0 }, { 0.0,5.0,5.0 }, { 0.0,0.0,0.0 }, { 0.0,0.2,0.2 }, 150, 1.0));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.999, mass, randColor, mass / 10.0f, 1000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0,-10000.0,0.0 }, { 0.0, 0.0, 0.0 }, { 0.0,0.0,0.0 }, { g }, 1, randColor, 0.4, 1000));
	}

	//esfera
	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		mass = randomMass();
		shared_ptr<ParticleGenerator> g(new SphereParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, 150));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.999, mass, randColor, mass / 10.0f, 1000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0, -10000.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0,0,0 }, { g }, 1, randColor, 0.4, 1000));
	}

	//random
	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		mass = randomMass();
		shared_ptr<ParticleGenerator> g(new GaussianParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, { 0,0,0 }, { 2.0,2.0,2.0 }, 150, 1.0));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.999, mass, randColor, mass / 10.0f, 1000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0, -10000.0, 0.0 }, { 0,0,0 }, { 0,0,0 }, { g }, 1, randColor, 0.4, 1000));
	}

	_firework_gen = new GaussianParticleGenerator({ 0,0,0 }, { 0.0,35.0,0.0 }, { 0,0,0 }, { 0.0,0.0,0.0 }, 1, 1000);

}



Vector4 PaintSystem::randomColor()
{
	return Vector4((rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, 1.0);
}

float PaintSystem::randomMass()
{
	return rand() % 4 + 1;
}


void PaintSystem::deleteBodies() {
	for (auto rb : _rigidBodies)
		delete rb;
	_rigidBodies.clear();
}

std::string PaintSystem::getInfo() noexcept {
	std::string display = "Paint System : ";

	display += "Thickness: " + std::to_string(_pincel->getDevPos().x * 10) + "/" + std::to_string(_maxThickness * 10);
	display += " | Color: (" + std::to_string(_color.x) + " " + std::to_string(_color.y) + " " + std::to_string(_color.z) + ")";
	display += " | Controls: LMB->brush   RMB->eraser   MMB->clear canvas   A->clear canvas   Q/W->increase/decrease thickness   S->screenshot";

	return display;
}
