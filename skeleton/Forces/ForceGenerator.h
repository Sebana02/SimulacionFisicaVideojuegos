#ifndef __FORCE_GENERATOR__
#define __FORCE_GENERATOR__

#include "../Particles/Particle.h"
#include <list>
#include <random>

class ForceGenerator
{
public:
	ForceGenerator() {};
	virtual ~ForceGenerator() {};
	virtual void updateForce(Particle* particle, double duration) = 0;
	inline std::string getName() { return _name; };
	std::string _name;
	double t = -1e10;//if starting negative --> eternal
};

class GravityForceGenerator : public ForceGenerator
{
public:
	GravityForceGenerator(const Vector3& gravity);
	virtual void updateForce(Particle* particle, double duration) override;
	inline void setGravity(Vector3 gravity) { _gravity = gravity; }
protected:
	Vector3 _gravity;
};

class ParticleDragGenerator : public ForceGenerator {
	
public:
	ParticleDragGenerator(const float k1, const float k2);
	virtual void updateForce(Particle* particle, double duration) override;
	inline void setDrag(float k1, float k2) noexcept { _k1 = k1; _k2 = k2; }


protected:
	float _k1;
	float _k2;
};

class WindForceGenerator : public ParticleDragGenerator
{
public:
	WindForceGenerator(double k1,double k2,const Vector3& wind, double radius, const Vector3& position);
	virtual ~WindForceGenerator();
	void updateForce(Particle* particle, double duration) override;
	inline void setWind(Vector3 wind) { _wind = wind; }
protected:
	Vector3 _wind, _position;
	double _action_radius;

	Particle* _particle = nullptr;

};

#endif // __FORCE_GENERATOR__


