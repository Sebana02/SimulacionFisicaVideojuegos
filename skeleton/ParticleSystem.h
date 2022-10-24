#ifndef __PARTICLE_SYSTEM__ 
#define __PARTICLE_SYSTEM__ 

#include "Particle.h"
#include "ParticleGenerator.h"
#include "core.hpp"

struct Payload {
	unsigned _type;
	unsigned _count;

	Payload(unsigned type, unsigned count) {
		_type = type;
		_count = count;
	}
	void set(unsigned type, unsigned count) {
		_type = type;
		_count = count;
	}
};

struct FireworkRules {
	unsigned _type;
	float _minAge;
	float _maxAge;

	Vector3 _minVelocity;
	Vector3 _maxVelocity;
	float _damping;

	std::vector <Payload> _payloads;

	void set(int type, float minAge, float maxAge, Vector3 minVelocity, Vector3 maxVelocity, float damping, std::vector<Payload> payloads) {
		_type = type;
		_minAge = minAge;
		_maxAge = maxAge;
		_minVelocity = minVelocity;
		_maxVelocity = maxVelocity;
		_damping = damping;
		_payloads = payloads;
	}
};

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

	std::vector<FireworkRules> _firework_rules;
	void createFireworkRules();

	std::vector<Firework*> _fireworks_pool;
	ParticleGenerator* _firework_gen;
};

#endif __PARTICLE_SYSTEM__

