#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {
	////generador
	//ParticleGenerator* gaussian_generator = new GaussianParticleGenerator({ 0.0,0.0,0.0 }, { 1.0,25.0,1.0 }, { 0.0,0.0,0.0 }, { 2.0,1.0,2.0 }, 100, 2);
	//gaussian_generator->setParticle(new Particle({ 0,0,0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.99, 1.0, { 0.5,0.5,1.0,1.0 }, 0.5, 10000, 500.0));
	//_particle_generators.push_back(gaussian_generator);

	//ParticleGenerator* uniform_generator = new UniformParticleGenerator({ 0,0,0 }, { 5,5,5 }, -10, 10);
	//uniform_generator->setParticle(new Particle({ 0,0,0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.99, 1.0, { 1.0,1.0,0.0,1.0 }, 1.0, 5000, 500.0));
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
			delete p;
		}
	}


	for (auto p : _particle_generators)
		_particles.push_back(*p->generateParticles().begin());
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

void ParticleSystem::generateFirework(unsigned type) {//para el primer firework
	if (type > _fireworks_pool.size())
		return;

	_firework_gen->setParticle(_fireworks_pool[type]);//
	auto& n_p = _firework_gen->generateParticles();
	_particles.push_back(*n_p.begin());
}

void ParticleSystem::generateFireworkSystem() {

}
void ParticleSystem::onParticleDeath(Particle* p) {
	if (p->_type == Particle::TYPE::FIREWORK) {
		Firework* f = (Firework*)p;
		if (f)
			_particles.push_back(*f->explode().begin());
	}
}