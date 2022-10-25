#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem() {

	generateFireworkSystem();

	generateFirework(3);

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

ParticleGenerator* ParticleSystem::getParticleGenerator(string name) {
	for (auto gen : _particle_generators)
		if (gen->getName() == name)
			return gen;

	return nullptr;
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

	//Particulas que simulan humo, no generan nada al morir
	auto base_particle = new Particle({ 0.0,0.0,0.0 }, { 0.0, 0.0, 0.0 }, _gravity, 0.999, 1.0, { 0.3, 0.2, 0.2, 0.8 }, 0.1, 1000, -1);


	//Firewrok cero, solo genera humillo
	shared_ptr<ParticleGenerator> g1(new GaussianParticleGenerator({ 0.1, 0.1, 0.1 }, { 0.0, 1.0, 1.0 }, { 0.1, 0.1, 0.1 }, { 1.0,1.0,1.0 }, 25));
	g1->setParticle(base_particle);
	g1->setName("BaseFireworkGenerator");
	Firework* f1 = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g1 }, 0.999, { 0.7, 0.3, 0.5, 1.0 }, 0.3, 250, 0);
	_fireworks_pool.push_back(f1);

	//Firework 1 --> genera fireworks 0
	shared_ptr<ParticleGenerator> g2(new GaussianParticleGenerator({ 0.1, 0.1, 0.1 }, { 0.0, 2.0, 2.0 }, { 0.1, 0.1, 0.1 }, { 1.0,1.0,1.0 }, 20));
	g2->setParticle(_fireworks_pool[0]->clone());
	g2->setName("FireworkGenerator1");
	Firework* f2 = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g2, g1 }, 0.999, { 0.4, 0.2, 0.8, 1.0 }, 0.5, 250, 1);
	_fireworks_pool.push_back(f2);

	//Genera Firework 2 y humo
	shared_ptr<ParticleGenerator> g3(new GaussianParticleGenerator({ 0.1, 0.1, 0.1 }, { 0.0, 3.0, 3.0 }, { 0.1, 0.1, 0.1 }, { 1.0,1.0,1.0 }, 15));
	g3->setParticle(_fireworks_pool[1]->clone());
	g3->setName("FireworkGenerator2");
	Firework* f3 = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g3 }, 0.999, { 0.2, 0.5, 0.2, 1.0 }, 0.6, 500, 2);
	_fireworks_pool.push_back(f3);

	shared_ptr<ParticleGenerator> g4(new GaussianParticleGenerator({ 0.1, 0.1, 0.1 }, { 0.0, 5.0, 5.0 }, { 0.1, 0.1, 0.1 }, { 1.0,1.0,1.0 }, 10));
	g4->setParticle(_fireworks_pool[2]->clone());
	g4->setName("FireworkGenerator3");
	Firework* f4 = new Firework({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, _gravity, { g4, g2 }, 0.999, { 0.2, 0.5, 0.3, 1.0 }, 1.0, 2000, 3);
	_fireworks_pool.push_back(f4);


	_firework_gen = new GaussianParticleGenerator({ 0,0,0 }, { 5.0,30.0,0.0 }, { 1.0, 0.2, 1.0 }, { 2.0,2.0,0.0 }, 1, 1);
	_firework_gen->setParticle(_fireworks_pool[3]->clone());

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