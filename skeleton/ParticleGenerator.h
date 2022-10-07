#ifndef __PARTICLE__GENERATOR__
#define __PARTICLE__GENERATOR__

#include <string>
#include <list>
#include "core.hpp"
#include "Particle.h"

using namespace std;

class ParticleGenerator
{
public:
	ParticleGenerator(string name, Vector3 mean_pos, Vector3 mean_vel);
	~ParticleGenerator();
	void setParticle(Particle* model);
	virtual list<Particle*>generateParticles() = 0;
protected:
	string name_;
	Vector3 _mean_pos, _mean_vel;
	double _generation_probability;
	int _num_particles;
	Particle* _model;
};

class GaussianParticleGenerator : public ParticleGenerator {
public:

};
#endif __PARTICLE__GENERATOR__

