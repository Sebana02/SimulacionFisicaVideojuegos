#ifndef __PARTICLE__GENERATOR__
#define __PARTICLE__GENERATOR__

#include <string>
#include <list>
#include "core.hpp"
#include "Particle.h"
#include <random>

#include "checkML.h"

using namespace std;

class ParticleGenerator
{
public:
	~ParticleGenerator();
	void setParticle(Particle* model);
	virtual list<Particle*>generateParticles() = 0;
	void setOrigin(Vector3 pos) { _mean_pos = pos; }
	double getProb() { return _generation_probability; }
	void setProb(double prob) { _generation_probability = prob; }
	bool isActive() { return _active; }
	void setActive(bool set) { _active = set; }

protected:
	Vector3 _mean_pos, _mean_vel;
	double _generation_probability;
	int _num_particles;
	Particle* _model = nullptr;
	bool _active = true;
};

class GaussianParticleGenerator : public ParticleGenerator {
public:
	GaussianParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, Vector3 dev_pos, Vector3 dev_vel,
		int num_particles = 10, double prob = 1);
	virtual std::list<Particle*> generateParticles() override;

protected:
	Vector3 std_dev_pos, std_dev_vel;

	std::random_device mch; //get each time a different distribution
	std::default_random_engine _gen{ mch() };
	std::normal_distribution<double> d{ 0,1 };
};

class CircleParticleGenerator : public GaussianParticleGenerator {
public :
	CircleParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, Vector3 dev_pos, Vector3 dev_vel,
		int num_particles = 10, double prob = 1)
		: GaussianParticleGenerator(mean_pos, mean_vel, dev_pos, dev_vel, num_particles, prob) {};
	virtual std::list<Particle*> generateParticles() override;

};

class UniformParticleGenerator : public ParticleGenerator {
public:
	UniformParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, double a, double b,
		int num_particles = 10, double prob = 1);
	virtual std::list<Particle*> generateParticles() override;

protected:
	double _a = -20, _b = 20;

	std::random_device mch;
	std::default_random_engine _gen{mch()};
	std::uniform_real_distribution<double> d{ _a,_b };
};

class SphereParticleGenerator : public UniformParticleGenerator {
public:
	SphereParticleGenerator(Vector3 mean_pos, Vector3 mean_vel,
		int num_particles = 10)
		:UniformParticleGenerator(mean_pos, mean_vel, 0, 1, num_particles, 1) {};
	virtual std::list<Particle*> generateParticles() override;

protected:
	std::random_device mch;
	std::default_random_engine _gen{ mch() };
	std::uniform_real_distribution<double> d{ 0,1 };
};
#endif __PARTICLE__GENERATOR__

