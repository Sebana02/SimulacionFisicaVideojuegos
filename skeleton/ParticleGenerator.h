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
	ParticleGenerator() {};
	~ParticleGenerator() {};
	void setParticle(Particle* model);
	virtual list<Particle*>generateParticles() = 0;
	void setOrigin(Vector3 pos) { _mean_pos = pos; }
	string getName() { return name_; }
protected:
	string name_;
	Vector3 _mean_pos, _mean_vel;
	double _generation_probability;
	int _num_particles;
	Particle* _model = nullptr;
};

class GaussianParticleGenerator : public ParticleGenerator {
public:
	GaussianParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, Vector3 dev_pos, Vector3 dev_vel, 
		int num_particles = 10, double prob = 1);
	~GaussianParticleGenerator() { if(_model != nullptr) delete _model; }
	virtual std::list<Particle*> generateParticles() override;

protected:
	Vector3 std_dev_pos, std_dev_vel;

	std::default_random_engine _gen;
	std::normal_distribution<double> d{ 0,1 };
};


class UniformParticleGenerator : public ParticleGenerator {
public:
	UniformParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, double a, double b,
		int num_particles = 10, double prob = 1);
	~UniformParticleGenerator() { if (_model != nullptr) delete _model; }
	virtual std::list<Particle*> generateParticles() override;

protected:
	double _a = -20, _b = 20;

	std::default_random_engine _gen;
	std::uniform_real_distribution<double> d{ _a,_b };
};
#endif __PARTICLE__GENERATOR__

