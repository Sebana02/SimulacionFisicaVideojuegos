#ifndef __PARTICLE_SYSTEM__ 
#define __PARTICLE_SYSTEM__ 

#include "Particle.h"
#include "ParticleGenerator.h"
#include "core.hpp"



class ParticleSystem
{
public:

	ParticleSystem();
	~ParticleSystem();
	void update(double t);
	ParticleGenerator* getParticleGenerator(string name);
	void generateFireworkSystem();
	void generateShot(Proyectile::TYPE proyectile_type, Vector3 pos, Vector3 dir, int lifeTime, double posDes);
	void generateFirework(unsigned type);
	void onParticleDeath(Particle* p);
protected:
	std::list<Particle*> _particles;
	std::list<ParticleGenerator*> _particle_generators;
	std::vector<Firework*> _fireworks_pool;
	ParticleGenerator* _firework_gen;
};

#endif __PARTICLE_SYSTEM__

