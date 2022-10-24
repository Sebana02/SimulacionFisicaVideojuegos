#ifndef __PARTICLE__ 
#define __PARTICLE__ 

#include "core.hpp"
#include "RenderUtils.hpp"
#include <memory>
#include <list>

using namespace physx;
using namespace std;


class Particle {

public:
	enum TYPE {
		NORMAL = 0,
		FIREWORK
	}_type = NORMAL;

	Particle(Vector3 position, Vector3 velocity, Vector3 accceleration, double damp, double mass,
		Vector4 color, double scale, int lifeTime, double posDes);
	~Particle();

	void integrate(double t);

	Vector3 getVel() { return _vel; }
	PxTransform& getTr() { return _tr; }
	Vector3 getAccel() { return _accel; }
	double getMass() { return 1.0 / _inverse_mass; }
	double getInverseMass() { return _inverse_mass; }
	double getDamping() { return _damping; }

	int getLifeTime() { return _lifeTime; }
	double getLifePos() { return _lifePos; }

	Vector4 getColor() { return _color; }
	double getScale() { return _scale; }

	RenderItem* getRenderItem() { return _renderItem; }

	bool isAlive() { return _alive; }

	void setVel(Vector3 vel) { _vel = vel; }


	//virtual Particle* clone() const;

protected:
	RenderItem* _renderItem;
	Vector3 _vel;
	PxTransform _tr; //pasarle a render item la direccion de esta
	Vector3 _accel;
	double _damping;
	double _inverse_mass;

	int _lifeTime;
	double _lifePos;

	bool _alive;

	Vector4 _color;
	double _scale;
};

class Proyectile : public Particle {
public:
	enum TYPE {
		PISTOL = 0,
		ARTILLERY,
		FIREBALL,
		LASER
	}_type;

	Proyectile(TYPE tipo, Vector3 pos, Vector3 dir, int lifeTime, double posDes);
};

class ParticleGenerator;//forward declaration

class Firework : public Particle {
public:
	Firework(Vector3 pos, Vector3 vel, Vector3 accel, std::list<std::shared_ptr<ParticleGenerator>> gens, double damp, double duration, unsigned type);
	~Firework() {};

	void integrate(double t);
	virtual Particle* clone()const;
	std::list<std::shared_ptr <ParticleGenerator>> _gens;
	std::list<Particle*> explode();

protected:
	unsigned _type;
};
#endif // !