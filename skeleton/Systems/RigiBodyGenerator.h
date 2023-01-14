#ifndef __RIGID_BODY__GENERATOR__
#define __RIGID_BODY__GENERATOR__

#include <string>
#include <list>
#include "../core.hpp"
#include "../Particles/Rigidbody.h"
#include <random>

#include "../checkML.h"

using namespace std;

class RigidBodyGenerator
{
public:
	virtual ~RigidBodyGenerator();
	void setParticle(Rigidbody* model);
	virtual list<Rigidbody*>generateParticles() = 0;
	inline void setOrigin(Vector3 pos) noexcept { _mean_pos = pos; };
	inline void setVelocity(Vector3 vel) noexcept { _mean_vel = vel; };
	inline bool isActive() noexcept { return _active; };
	inline void setActive(bool set) noexcept { _active = set; };
	inline void setParticlesNum(int num) noexcept { _num_particles = num; };

protected:
	Vector3 _mean_pos, _mean_vel;
	double _generation_probability;
	int _num_particles;
	Rigidbody* _model = nullptr;
	bool _active = true;
};


class GaussianRBGenerator : public RigidBodyGenerator {
public:
	GaussianRBGenerator(Vector3 mean_pos, Vector3 mean_vel, Vector3 dev_pos, Vector3 dev_vel,
		int num_particles = 10, double prob = 1);
	virtual ~GaussianRBGenerator() {};
	virtual std::list<Rigidbody*> generateParticles() override;
	inline void setDevPos(Vector3 dev) noexcept { std_dev_pos = dev; };
	inline Vector3 getDevPos() { return std_dev_pos; };

protected:
	Vector3 std_dev_pos, std_dev_vel;

	std::random_device mch; //get each time a different distribution
	std::default_random_engine _gen{ mch() };
	std::normal_distribution<double> d{ 0,1 };

};

#endif __RIGID_BODY__GENERATOR__

