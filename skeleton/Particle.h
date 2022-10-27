#ifndef __PARTICLE__ 
#define __PARTICLE__ 

#include "core.hpp"
#include "RenderUtils.hpp"
#include <memory>
#include <list>

#include "checkML.h"

using namespace physx;
using namespace std;


class Particle {

public:
	enum TYPE {
		NORMAL = 0,
		FIREWORK
	}_type;

	Particle(Vector3 position, Vector3 velocity, Vector3 accceleration, double damp, double mass,
		Vector4 color, double scale, int lifeTime, double posDes);
	~Particle();

	void integrate(double t);

	RenderItem* getRenderItem() { return _renderItem; }

	bool isAlive() { return _alive; }

	Vector3 getVel() { return _vel; }
	void setVel(Vector3 vel) { _vel = vel; }
	void setPos(Vector3 pos) { _tr.p = pos; }

	virtual Particle* clone() const;

protected:
	RenderItem* _renderItem;
	Vector3 _vel;
	PxTransform _tr; //pasarle a render item la direccion de esta
	Vector3 _accel;
	double _damping;
	double _inverse_mass;

	int _lifeTime;
	double _lifePos, _duration;

	bool _alive;

	Vector4 _color;
	double _scale;
};

class Proyectile : public Particle {
public:
	enum PROYECTILE_TYPE {
		PISTOL = 0,
		ARTILLERY,
		FIREBALL,
		LASER
	}_proyectile_type;

	Proyectile(PROYECTILE_TYPE tipo, Vector3 pos, Vector3 dir, int lifeTime, double posDes);
};

class ParticleGenerator;//forward declaration

class Firework : public Particle {
public:
	Firework(Vector3 pos, Vector3 vel, Vector3 accel, std::list<std::shared_ptr<ParticleGenerator>> gens, double damp, Vector4 color, double scale, double duration);
	
	virtual Firework* clone()const override;
	std::list<std::shared_ptr <ParticleGenerator>> _gens;
	std::list<Particle*> explode();

};
#endif // !