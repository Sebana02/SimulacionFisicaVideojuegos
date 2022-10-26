#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem() {

	generateFireworkSystem();

	generateFirework(0);


	//generador
	/*Vector3 mean_pos = { 0,0,0 }, meanVel = { 1.0,25.0,1.0 };
	ParticleGenerator* gaussian_generator = new GaussianParticleGenerator(mean_pos, meanVel, { 0.0,0.0,0.0 }, { 2.0,1.0,2.0 }, 100, 0.1);
	gaussian_generator->setParticle(new Particle(mean_pos, meanVel, _gravity, 0.99, 1.0, { 0.5,0.5,1.0,1.0 }, 0.5, 10000, 300));
	_particle_generators.push_back(gaussian_generator);*/


	//ParticleGenerator* uniform_generator = new UniformParticleGenerator(mean_pos, meanVel, -10, 10, 100, 0.1);
	//uniform_generator->setParticle(new Particle(mean_pos, meanVel, _gravity, 0.99, 1.0, { 1.0,1.0,0.0,1.0 }, 1.0, 5000, 500.0));
	//_particle_generators.push_back(uniform_generator);

	////suelo
	//Particle* suelo = new Particle({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 0, 0, { 1.0,0.0,0.0,1.0 }, 1.0, 0, 0);
	//RenderItem* renderItem = suelo->getRenderItem();
	//renderItem->shape = CreateShape(physx::PxBoxGeometry(100.0f, 1.0f, 100.0f));
	//_particles.push_back(suelo);


}

ParticleSystem::~ParticleSystem() {
	for (auto gen : _particle_generators)
		delete gen;
	_particle_generators.clear();

	for (auto particle : _particles)
		delete particle;
	_particles.clear();

	delete _firework_gen;

	for (auto firework : _fireworks_pool)
		delete firework;
	_fireworks_pool.clear();
}

void ParticleSystem::update(double t) {

	for (auto p : _particle_generators) {
		std::list<Particle*> particles = p->generateParticles();
		for (auto particle : particles)
			_particles.push_back(particle);
		particles.clear();
	}

	//update de las particulas
	std::list<Particle*>::iterator it = _particles.begin();
	while (it != _particles.end()) {
		Particle* p = *it;
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

void ParticleSystem::generateShot(Proyectile::TYPE proyectile_type, Vector3 pos, Vector3 dir, int lifeTime, double posDes) {
	_particles.push_back(new Proyectile(proyectile_type, pos, dir, lifeTime, posDes));
}

void ParticleSystem::generateFirework(unsigned type) {//para el primer firework??
	if (type > _fireworks_pool.size())
		return;

	_firework_gen->setParticle(_fireworks_pool[type]->clone());

	std::list<Particle*> particles = _firework_gen->generateParticles();
	for (auto p : particles)
		_particles.push_back(p);

	particles.clear();
}

void ParticleSystem::generateFireworkSystem() {

	//Particle* base_particle = new Particle({ 0.0,0.0,0.0 }, { 0.0, 0.0, 0.0 }, _gravity, 0.999, 1.0, { 0.3, 0.2, 0.2, 0.8 }, 0.2, 5000, -1);
	//shared_ptr<ParticleGenerator> g1(new CircleParticleGenerator({ 0.0,0.0,0.0 }, { 0.0,5.0,5.0 }, { 0.0,0.0,0.0 }, { 0.0,0.2,0.2 }, 20, 1.0));
	//g1->setParticle(base_particle);
	//Firework* firework = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g1 }, 0.999, { 0.7, 0.3, 0.5, 1.0 }, 0.4, 1000);
	//shared_ptr<ParticleGenerator> g2(new CircleParticleGenerator({ 0.0,0.0,0.0 }, { 0.0,5.0,5.0 }, { 0.0,0.0,0.0 }, { 0.0,0.2,0.2 }, 20, 1.0));
	//g2->setParticle(firework);
	//Firework* firework2 = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g2 }, 0.999, { 0.9, 0.5, 0.7, 1.0 }, 0.6, 2000);
	//_fireworks_pool.push_back(firework2);


	Particle* base_particle = new Particle({ 0.0,0.0,0.0 }, { 0.0, 0.0, 0.0 }, _gravity, 0.999, 1.0, { 0.9, 0.5, 0.7, 1.0 }, 0.2, 3000, -1);
	shared_ptr<ParticleGenerator> g1(new SphereParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, 10));
	g1->setParticle(base_particle);
	Firework* firework = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g1 }, 0.999, { 0.9, 0.5, 0.7, 1.0 }, 0.4, 2000);
	shared_ptr<ParticleGenerator> g2(new SphereParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, 100));
	g2->setParticle(firework);
	Firework* firework2 = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g2 }, 0.999, { 0.9, 0.5, 0.7, 1.0 }, 0.6, 2000);
	_fireworks_pool.push_back(firework2);

	_firework_gen = new GaussianParticleGenerator({ 0,0,0 }, { 0.0,35.0,0.0 }, { 0.0, 0.0,0.0 }, { 0.0,1.0,5.0 }, 10, 2);
}
void ParticleSystem::onParticleDeath(Particle* p) {
	if (p->_type == Particle::TYPE::FIREWORK) {
		Firework* firework = (Firework*)p;
		if (firework != nullptr) {
			std::list<Particle*> particles = firework->explode();
			for (auto p : particles)
				_particles.push_back(p);
			particles.clear();
		}
	}
	delete p;
}