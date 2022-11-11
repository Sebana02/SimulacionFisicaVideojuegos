#ifndef __PARTICLE_SYSTEM__ 
#define __PARTICLE_SYSTEM__ 

#include "Particle.h"
#include "ParticleGenerator.h"
#include "core.hpp"
#include "ParticleForceRegistry.h"
#include "ForceGenerator.h"
#include <random>
#include <memory>
#include <list>

#include "checkML.h"

using namespace std;

class ParticleSystem
{
public:

	ParticleSystem();
	void generateForceGenerators();
	~ParticleSystem();
	void update(double t);
	void generateFireworkSystem();
	void generateShot(Proyectile::PROYECTILE_TYPE proyectile_type, Vector3 pos, Vector3 dir, int lifeTime, double posDes);
	void generateFirework(unsigned type);
	void onParticleDeath(Particle* p);
	void addGenerator(ParticleGenerator* gen);
	ParticleGenerator* getGenerator(int i);
	inline void changeSpawnFireworks() noexcept{_spawn_fireworks = !_spawn_fireworks;};
	void changeGravity();
protected:
	std::list<Particle*> _particles;
	std::list<ParticleGenerator*> _particle_generators;
	
	std::list<ForceGenerator*> _force_generators;
	ParticleForceRegistry* _registry = nullptr;
	
	std::vector<Firework*> _fireworks_pool;
	ParticleGenerator* _firework_gen = nullptr;
	bool _spawn_fireworks;
	int _next_firework;

	bool _gravity_active;

	Vector4 randomColor();
};

#endif __PARTICLE_SYSTEM__

