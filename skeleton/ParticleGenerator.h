#ifndef __PARTICLE__GENERATOR__
#define __PARTICLE__GENERATOR__

#include <string>
#include <list>
#include "core.hpp"
#include "Particle.h"
#include <random>

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
	GaussianParticleGenerator(string name, Vector3 mean_pos, Vector3 mean_vel,Vector3 std_dev_pos, Vector3 std_dev_vel);

	virtual std::list<Particle*> generateParticles();

protected:
	Vector3 std_dev_pos, std_dev_vel;
	double std_dev_t;

	std::normal_distribution<double> d{ 0,1 };
}; 
#endif __PARTICLE__GENERATOR__

