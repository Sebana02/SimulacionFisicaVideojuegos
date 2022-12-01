#ifndef __PARTICLE_SYSTEM__ 
#define __PARTICLE_SYSTEM__ 

#include "../Particles/Particle.h"
#include "ParticleGenerator.h"
#include "../core.hpp"
#include "../Forces/ParticleForceRegistry.h"
#include "../Forces/ForceGenerator.h"
#include <random>
#include <memory>
#include <list>

#include "../checkML.h"

using namespace std;

class ParticleSystem
{
public:

	ParticleSystem();
	~ParticleSystem();
	void update(double t);
	void generateFireworkSystem();
	void generateShot(Proyectile::PROYECTILE_TYPE proyectile_type, Vector3 pos, Vector3 dir, int lifeTime, double posDes);
	void generateFirework(unsigned type);
	void onParticleDeath(Particle* p);
	
	void addGenerator(ParticleGenerator* gen);
	ParticleGenerator* getGenerator(int i);
	
	inline void changeSpawnFireworks() noexcept{_spawn_fireworks = !_spawn_fireworks;};
	
	void addGravity();
	void deleteGravity();

	void addWind();
	void deleteWind();

	void addWhirlwind();
	void deleteWhirlwind();

	void addExplosion();
	void deleteExplosion();
	
	void addParticles(std::list<Particle*>& list);
	Vector4 randomColor();

	void generateSpringDemo();
	void generateAnchoredSpringDemo();
	void generateBungeeSpringDemo();
	void generateBuoyancyDemo();
	void generateSlinkyDemo();

protected:
	std::list<Particle*> _particles;
	std::list<ParticleGenerator*> _particle_generators;
	
	GravityForceGenerator* gravity_force = nullptr;
	WindForceGenerator* wind_force = nullptr;
	WhirlwindForceGenerator* whirlwind_force = nullptr;
	ExplosionForceGenerator* explosion_force = nullptr;
	ParticleForceRegistry* _registry = nullptr;
	
	std::list<ForceGenerator*> _springs_forces;

	
	std::vector<Firework*> _fireworks_pool;
	ParticleGenerator* _firework_gen = nullptr;
	bool _spawn_fireworks;
	int _next_firework;
};

#endif __PARTICLE_SYSTEM__

