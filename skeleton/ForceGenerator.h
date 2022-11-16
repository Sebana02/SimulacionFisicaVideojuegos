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
	inline std::string getName() { return _name; };
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

class WindForceGenerator : public ForceGenerator
{
public:
	WindForceGenerator(const Vector3& wind, double radius, const Vector3& position);
	~WindForceGenerator();
	void updateForce(Particle* particle, double duration) override;
	inline void setWind(Vector3 wind) { _wind = wind; }
	inline RenderItem* getRenderItem() noexcept { return _renderItem; }
protected:
	Vector3 _wind, _position;
	double _action_radius;

	RenderItem* _renderItem = nullptr;
};

#endif // __FORCE_GENERATOR__


