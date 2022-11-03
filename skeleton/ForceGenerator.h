#ifndef __FORCE_GENERATOR__
#define __FORCE_GENERATOR__

#include "Particle.h"
#include <list>
#include <random>

class ForceGenerator
{
public:
	ForceGenerator() {};
	virtual ~ForceGenerator() {};
	virtual void updateForce(Particle* particle, double duration) = 0;
	std::string _name;
	double t = -1e10;//if starting negative --> eternal
};

class GravityForceGenerator : public ForceGenerator
{
public:
	GravityForceGenerator(const Vector3& gravity);
	void updateForce(Particle* particle, double duration) override;
	inline void setGravity(Vector3 gravity) { _gravity = gravity; }
protected:
	Vector3 _gravity;
};
#endif // __FORCE_GENERATOR__


