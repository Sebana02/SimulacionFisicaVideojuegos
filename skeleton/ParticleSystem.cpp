#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem() {


	//fuente
	ParticleGenerator* fuente = new GaussianParticleGenerator({0,0,0}, {1.0,25.0,1.0}, {0.0,0.0,0.0}, {2.0,1.0,2.0}, 100, 0.1);
	fuente->setParticle(new Particle({0.0,-10000.0,0.0}, {0,0,0}, {0,0,0}, 0.99, 1.0, randomColor(), 0.3, 5000, 300));
	_particle_generators.push_back(fuente);
	fuente->setActive(false);

	//humo
	ParticleGenerator* humo = new UniformParticleGenerator({ 0.0,20.0,0.0 },{0,0,0}, -10,10, 400, 0.1);
	humo->setParticle(new Particle({ 0.0,-10000.0,0.0 }, {0,0,0}, {0,0,0}, 0.99, 1.0, randomColor(), 0.3, 2000, 200.0));
	_particle_generators.push_back(humo);
	humo->setActive(false);
	
	generateFireworkSystem();
	
	generateForceGenerators();
	
	_registry = new ParticleForceRegistry();

	/*//suelo
	Particle* suelo = new Particle({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 0, 0, { 1.0,0.0,0.0,1.0 }, 1.0, 0, 0);
	RenderItem* renderItem = suelo->getRenderItem();
	renderItem->shape = CreateShape(physx::PxBoxGeometry(100.0f, 1.0f, 100.0f));
	_particles.push_back(suelo);*/

}

void ParticleSystem::generateForceGenerators()
{
	_force_generators.push_back(new GravityForceGenerator(_gravity));
}

ParticleSystem::~ParticleSystem() {
	for (auto firework : _fireworks_pool)
		delete firework;
	_fireworks_pool.clear();

	delete _firework_gen;

	for (auto gen : _particle_generators)
		delete gen;
	_particle_generators.clear();

	for (auto particle : _particles)
		delete particle;
	_particles.clear();

	for (auto force : _force_generators)
		delete force;

}

void ParticleSystem::update(double t) {

	//generar fireworks
	if (_spawn_fireworks && glutGet(GLUT_ELAPSED_TIME) > _next_firework) {
		if (_fireworks_pool.size() > 0) {
			generateFirework(rand() % _fireworks_pool.size());
			_next_firework = glutGet(GLUT_ELAPSED_TIME) + rand() % 500 + 500;
		}
	}

	//generate particles
	for (auto p : _particle_generators) {
		if (!p)continue;
		
		std::list<Particle*> particles = p->generateParticles();
		for (auto particle : particles) {
			
			_particles.push_back(particle);
		}
		particles.clear();
	}

	//update de las particulas
	std::list<Particle*>::iterator it = _particles.begin();
	while (it != _particles.end()) {
		Particle* p = *it;
		if (!p)continue;
		if (p->isAlive()) {
			p->integrate(t);
			++it;
		}
		else {
			onParticleDeath(p);
			it = _particles.erase(it);
		}
	}
}

void ParticleSystem::generateShot(Proyectile::PROYECTILE_TYPE proyectile_type, Vector3 pos, Vector3 dir, int lifeTime, double posDes) {
	_particles.push_back(new Proyectile(proyectile_type, pos, dir, lifeTime, posDes));
}

void ParticleSystem::generateFirework(unsigned type) {//para el primer firework??
	if (type > _fireworks_pool.size())
		return;

	_firework_gen->setParticle(_fireworks_pool.at(type)->clone());

	for (auto p : _firework_gen->generateParticles())
		_particles.push_back(p);

}

void ParticleSystem::generateFireworkSystem() {

	_next_firework = 0;
	_spawn_fireworks = false;

	//circulo
	Vector4 randColor;
	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		shared_ptr<ParticleGenerator> g(new CircleParticleGenerator({ 0.0,0.0,0.0 }, { 0.0,5.0,5.0 }, { 0.0,0.0,0.0 }, { 0.0,0.2,0.2 }, 150, 1.0));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, {0,0,0}, {0,0,0}, 0.999, 1.0, randColor, 0.2, 5000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0,-10000.0,0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g }, 0.999, randColor, 0.4, rand()%1000 + 1000));
	}

	//esfera
	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		shared_ptr<ParticleGenerator> g(new SphereParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, 150));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, {0,0,0}, {0,0,0}, 0.999, 1.0, randColor, 0.2, 4000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0, -10000.0, 0.0 }, { 0.0, 0.0, 0.0 }, {0,0,0}, {g}, 0.999, randColor, 0.4, rand() % 1000 + 1000));
	}

	//random
	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		shared_ptr<ParticleGenerator> g(new GaussianParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, {0,0,0}, {2.0,2.0,2.0}, 150, 1.0));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, {0,0,0}, {0,0,0}, 0.999, 1.0, randColor, 0.2, 3000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0, -10000.0, 0.0 }, {0,0,0}, {0,0,0}, {g}, 0.999, randColor, 0.4, rand() % 1000 + 1000));
	}

	_firework_gen = new GaussianParticleGenerator({0,0,0}, {0.0,35.0,0.0}, {0,0,0}, {0.0,1.0,5.0}, 1, 2);
}

void ParticleSystem::onParticleDeath(Particle* p) {
	if (p->_type == Particle::TYPE::FIREWORK) {
		Firework* firework = dynamic_cast<Firework*>(p);
		if (firework != nullptr) {
			std::list<Particle*> particles = firework->explode();
			for (auto par : particles)
				_particles.push_back(par);
			particles.clear();
		}
	}
	delete p;
}

void ParticleSystem::addGenerator(ParticleGenerator* gen)
{
	_particle_generators.push_back(gen);
}

ParticleGenerator* ParticleSystem::getGenerator(int i)
{
	if (i < _particle_generators.size() && i >= 0) {
		list<ParticleGenerator*>::iterator it = _particle_generators.begin();
		for (int j = 0; j < i; j++) 
			++it;
		
		return *it;
	}

	return nullptr;
}

Vector4 ParticleSystem::randomColor()
{
	return Vector4((rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, 1.0);
}