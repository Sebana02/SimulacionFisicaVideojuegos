#ifndef __FORCE_GENERATOR__
#define __FORCE_GENERATOR__

#include "../Particles/Particle.h"
#include <list>
#include <random>
#include <numbers>

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
	WindForceGenerator(double k1, double k2, const Vector3& wind, double radius, const Vector3& position);
	virtual ~WindForceGenerator();
	virtual void updateForce(Particle* particle, double duration) override;
	inline void setWind(Vector3 wind) { _wind = wind; }
protected:
	Vector3 _wind, _position;
	double _action_radius;

	Particle* _particle = nullptr;

};

class WhirlwindForceGenerator : public WindForceGenerator {
public:
	WhirlwindForceGenerator(double k1, double k2, int K, double radius, const Vector3& position);
	virtual ~WhirlwindForceGenerator() {};
	virtual void updateForce(Particle* particle, double duration) override;
protected:
	int _K;
};


class ExplosionForceGenerator : public ForceGenerator {
public:
	ExplosionForceGenerator(int K, const Vector3& position, float const_explosion);
	virtual ~ExplosionForceGenerator() {};
	virtual void updateForce(Particle* particle, double duration) override;
protected:
	int _K;
	double _radius;
	Vector3 _position;
	float _const_explosion;
	float vel_expansion = 343.4;
};

class SpringForceGenerator : public ForceGenerator {
public:
	SpringForceGenerator(double K, double resting_length, Particle* other);
	virtual ~SpringForceGenerator() {};
	virtual void updateForce(Particle* particle, double duration) override;
	inline void setK(double K) { _K = K; }

protected:
	double _K;
	double _resting_length;
	Particle* _other;
};
class AnchoredSpringFG : public SpringForceGenerator {
public:
	AnchoredSpringFG(double K, double resting_length, const Vector3& anchor);
	virtual ~AnchoredSpringFG();
};

class BungeeForceGenerator : public SpringForceGenerator {
public:
	BungeeForceGenerator(double K, double resting_length, Particle* other);
	virtual ~BungeeForceGenerator() {};
	virtual void updateForce(Particle* particle, double duration) override;
};


class BuoyancyForceGenerator : public ForceGenerator {
public:
	BuoyancyForceGenerator(float h, float V, float d);
	virtual ~BuoyancyForceGenerator() { };
	virtual void updateForce(Particle* particle, double duration) override;

protected:
	float _height;
	float _volume;
	float _liquid_density;
	float _gravity;

	Particle* _liquid_particle;
};
#endif // __FORCE_GENERATOR__


