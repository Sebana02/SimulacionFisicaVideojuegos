#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem() {


	//fuente
	ParticleGenerator* fuente = new GaussianParticleGenerator({ 0,0,0 }, { 1.0,25.0,1.0 }, { 0.0,0.0,0.0 }, { 2.0,1.0,2.0 }, 100, 0.1);
	fuente->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.99, 1.0, randomColor(), 0.3, -1, 400));
	_particle_generators.push_back(fuente);
	fuente->setActive(false);

	//humo
	ParticleGenerator* humo = new UniformParticleGenerator({ 0.0,20.0,0.0 }, { 0,0,0 }, -10, 10, 400, 0.1);
	humo->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.99, 1.0, randomColor(), 0.3, -1, 400));
	_particle_generators.push_back(humo);
	humo->setActive(false);

	generateFireworkSystem();

	_registry = new ParticleForceRegistry();
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
		if (particle != nullptr) delete particle;
	_particles.clear();

	if (gravity_force != nullptr) delete gravity_force;
	if (wind_force != nullptr)delete wind_force;
	if (whirlwind_force != nullptr)delete whirlwind_force;
	if (explosion_force != nullptr)delete explosion_force;
	for (auto spring : _springs_forces)
		if (spring != nullptr) delete spring;

	delete _registry;

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
	for (auto p : _particle_generators)
		addParticles(p->generateParticles());


	_registry->updateForces(t);

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

void ParticleSystem::generateFirework(unsigned type) {
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
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.999, 1.0, randColor, 0.2, 5000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0,-10000.0,0.0 }, { 0.0, 0.0, 0.0 }, { 0.0,-9.8,0.0 }, { g }, 0.999, randColor, 0.4, rand() % 1000 + 1000));
	}

	//esfera
	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		shared_ptr<ParticleGenerator> g(new SphereParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, 150));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.999, 1.0, randColor, 0.2, 4000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0, -10000.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0,-9.8,0 }, { g }, 0.999, randColor, 0.4, rand() % 1000 + 1000));
	}

	//random
	for (int i = 0; i < 5; i++) {
		randColor = randomColor();
		shared_ptr<ParticleGenerator> g(new GaussianParticleGenerator({ 0.0,0.0,0.0 }, { 5.0,5.0,5.0 }, { 0,0,0 }, { 2.0,2.0,2.0 }, 150, 1.0));
		g->setParticle(new Particle({ 0.0,-10000.0,0.0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.999, 1.0, randColor, 0.2, 3000, -1));
		_fireworks_pool.push_back(new Firework({ 0.0, -10000.0, 0.0 }, { 0,0,0 }, { 0,-9.8,0 }, { g }, 0.999, randColor, 0.4, rand() % 1000 + 1000));
	}

	_firework_gen = new GaussianParticleGenerator({ 0,0,0 }, { 0.0,35.0,0.0 }, { 0,0,0 }, { 0.0,1.0,5.0 }, 1, 2);
}

void ParticleSystem::onParticleDeath(Particle* p) {
	if (p->_type == Particle::TYPE::FIREWORK) {
		Firework* firework = dynamic_cast<Firework*>(p);
		if (firework != nullptr) {
			addParticles(firework->explode());
		}
	}
	_registry->deleteParticleRegistry(p);
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


void ParticleSystem::addGravity() {

	deleteGravity();

	gravity_force = new GravityForceGenerator({ 0.0,-9.8,0.0 });
	for (auto& p : _particles)
		_registry->addRegistry(gravity_force, p);

}

void ParticleSystem::deleteGravity() {
	if (gravity_force != nullptr) {
		_registry->deleteForce(gravity_force);
		delete gravity_force;
		gravity_force = nullptr;
	}
}

void ParticleSystem::addWind() {

	deleteWind();

	wind_force = new WindForceGenerator(0.9, 0.1, { 0.0,10.0,10.0 }, 200, { 0.0,75.0,0.0 });
	for (auto& p : _particles)
		_registry->addRegistry(wind_force, p);
}

void ParticleSystem::deleteWind() {
	if (wind_force != nullptr) {
		_registry->deleteForce(wind_force);
		delete wind_force;
		wind_force = nullptr;
	}
}

void ParticleSystem::addWhirlwind()
{
	deleteWhirlwind();

	whirlwind_force = new WhirlwindForceGenerator(1, 0.0, 1, 200, { 0.0,0.0,0.0 });
	for (auto& p : _particles)
		_registry->addRegistry(whirlwind_force, p);

}

void ParticleSystem::deleteWhirlwind()
{
	if (whirlwind_force != nullptr) {
		_registry->deleteForce(whirlwind_force);
		delete whirlwind_force;
		whirlwind_force = nullptr;
	}
}

void ParticleSystem::addExplosion()
{
	deleteExplosion();
	explosion_force = new ExplosionForceGenerator(200000, { 0.0,0.0,0.0 }, 2);
	for (auto& p : _particles)
		_registry->addRegistry(explosion_force, p);
}

void ParticleSystem::deleteExplosion()
{
	if (explosion_force != nullptr) {
		_registry->deleteForce(explosion_force);
		delete explosion_force;
		explosion_force = nullptr;
	}
}



Vector4 ParticleSystem::randomColor()
{
	return Vector4((rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, (rand() % 9 + 1) / 10.0, 1.0);
}



void ParticleSystem::generateSpringDemo()
{
	Particle* p1 = new Particle({ 0.0,60.0,-10.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.85, 2.0, { 0.5,0.5,1.0,1.0 }, 2.0, -1, -1);
	Particle* p2 = new Particle({ 0.0,20.0,10.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.85, 2.0, { 0.5,0.5,1.0,1.0 }, 2.0, -1, -1);

	SpringForceGenerator* spring_force1 = new SpringForceGenerator(1, 20, p2);
	_registry->addRegistry(spring_force1, p1);
	SpringForceGenerator* spring_force2 = new SpringForceGenerator(1, 20, p1);
	_registry->addRegistry(spring_force2, p2);

	_springs_forces.push_back(spring_force1);
	_springs_forces.push_back(spring_force2);
	_particles.push_back(p1);
	_particles.push_back(p2);
}

void ParticleSystem::generateAnchoredSpringDemo()
{
	Particle* p3 = new Particle({ 0.0,30.0,30.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.85, 2.0, { 1.0,1.0,1.0,1.0 }, 2.0, -1, -1);
	AnchoredSpringFG* f3 = new AnchoredSpringFG(1, 10, { 0.0,50.0,30.0 });
	_registry->addRegistry(f3, p3);
	_springs_forces.push_back(f3);
	_particles.push_back(p3);
}

void ParticleSystem::generateBungeeSpringDemo()
{
	Particle* p1 = new Particle({ 0.0,60.0,-30.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.85, 2.0, { 0.5,1.0,0.5,1.0 }, 2.0, -1, -1);
	Particle* p2 = new Particle({ 0.0,20.0,-10.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.85, 2.0, { 0.5,1.0,0.5,1.0 }, 2.0, -1, -1);

	BungeeForceGenerator* spring_force1 = new BungeeForceGenerator(1, 20, p2);
	_registry->addRegistry(spring_force1, p1);
	BungeeForceGenerator* spring_force2 = new BungeeForceGenerator(1, 20, p1);
	_registry->addRegistry(spring_force2, p2);

	_springs_forces.push_back(spring_force1);
	_springs_forces.push_back(spring_force2);
	_particles.push_back(p1);
	_particles.push_back(p2);
}

void ParticleSystem::generateBuoyancyDemo()
{
	Particle* cubo = new Particle({ 0.0,20.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.99, 2.0, { 1.0,0.5,0.5,1.0 }, 2.0, -1, -1);
	cubo->getRenderItem()->shape = CreateShape(physx::PxBoxGeometry(5.0, 5.0, 5.0));
	_particles.push_back(cubo);

	BuoyancyForceGenerator* force = new BuoyancyForceGenerator(0, 0.5, 1000);
	_registry->addRegistry(force, cubo);
	GravityForceGenerator* gravity = new GravityForceGenerator({ 0.0,-9.8,0.0 });
	_registry->addRegistry(gravity, cubo);


	_springs_forces.push_back(gravity);
	_springs_forces.push_back(force);
}

void ParticleSystem::generateSlinkyDemo()
{
	Particle* p1 = new Particle({ 0.0, 70.0, 20.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.5, 2.0, { 0.5,1.0,0.5,1.0 }, 2.0, -1, -1);
	AnchoredSpringFG* f1 = new AnchoredSpringFG(1, 2, {0, 75.0, 20.0 });
	_registry->addRegistry(f1, p1);
	_springs_forces.push_back(f1);
	_particles.push_back(p1);

	Particle* p2 = new Particle({ 0.0, 65.0, 20.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.5, 2.0, { 0.4,0.0,1.0,1.0 }, 2.0, -1, -1);
	SpringForceGenerator* f2 = new SpringForceGenerator(1, 2, p1);
	_registry->addRegistry(f2, p2);
	_springs_forces.push_back(f2);
	_particles.push_back(p2);

	Particle* p3 = new Particle({ 0.0, 60.0, 20.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.5, 2.0, { 0.7,0.0,1.0,1.0 }, 2.0, -1, -1);
	SpringForceGenerator* f3 = new SpringForceGenerator(1, 2, p2);
	_registry->addRegistry(f3, p3);
	_springs_forces.push_back(f3);
	_particles.push_back(p3);

	Particle* p4 = new Particle({ 0.0, 55.0, 20.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.5, 2.0, { 1.0,0.0,1.0,1.0 }, 2.0, -1, -1);
	SpringForceGenerator* f4 = new SpringForceGenerator(1, 2, p3);
	_registry->addRegistry(f4, p4);
	_springs_forces.push_back(f4);
	_particles.push_back(p4);

	Particle* p5 = new Particle({ 0.0, 50.0, 20.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.5, 2.0, { 1.0,0.0,0.7,1.0 }, 2.0, -1, -1);
	SpringForceGenerator* f5 = new SpringForceGenerator(1, 2, p4);
	_registry->addRegistry(f5, p5);
	_springs_forces.push_back(f5);
	_particles.push_back(p5);

	Particle* p6 = new Particle({ 0.0, 45.0, 20.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.5, 2.0, { 1.0,0.0,0.4,1.0 }, 2.0, -1, -1);
	SpringForceGenerator* f6 = new SpringForceGenerator(1, 2, p5);
	_registry->addRegistry(f6, p6);
	_springs_forces.push_back(f6);
	_particles.push_back(p6);

	Particle* p7 = new Particle({ 0.0, 40.0, 20.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.5, 2.0, { 1.0,0.0,0.1,1.0 }, 2.0, -1, -1);
	SpringForceGenerator* f7 = new SpringForceGenerator(1, 2, p6);
	_registry->addRegistry(f7, p7);
	_springs_forces.push_back(f7);
	_particles.push_back(p7);

	GravityForceGenerator* gravity = new GravityForceGenerator({ 0.0,-9.8,0.0 });
	_springs_forces.push_back(gravity);

	_registry->addRegistry(gravity, p1);
	_registry->addRegistry(gravity, p2);
	_registry->addRegistry(gravity, p3);
	_registry->addRegistry(gravity, p4);
	_registry->addRegistry(gravity, p5);
	_registry->addRegistry(gravity, p6);
	_registry->addRegistry(gravity, p7);
	
}


void ParticleSystem::addParticles(std::list<Particle*>& list) {
	for (auto particle : list) {

		if (gravity_force != nullptr)
			_registry->addRegistry(gravity_force, particle);
		if (wind_force != nullptr)
			_registry->addRegistry(wind_force, particle);
		if (whirlwind_force != nullptr)
			_registry->addRegistry(whirlwind_force, particle);
		if (explosion_force != nullptr)
			_registry->addRegistry(explosion_force, particle);

		_particles.push_back(particle);
	}
	list.clear();
}