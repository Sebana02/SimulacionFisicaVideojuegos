#ifndef __PARTICLE__ 
#define __PARTICLE__ 

#include "core.hpp"
#include "RenderUtils.hpp"

using namespace physx;

class Particle {

public:
	Particle(Vector3 position, Vector3 velocity, Vector3 accceleration, double damp, double mass,
				Vector4 color, double scale, int lifeTime, double posDes);
	~Particle();

	void integrate(double t);

	void setVelocity(Vector3 velocity) { _vel = velocity; }
	void setAcceleration(Vector3 acceleration) { _accel = acceleration; }
	void setDamping(double damp) { _damping = damp; }
	void setMass(double mass) { _inverse_mass = 1.0/mass; }
	void setPos(Vector3 pos) { _tr.p = pos; }

	Vector3 getVel() { return _vel; }
	PxTransform& getTr() { return _tr; }
	Vector3 getAccel() { return _accel; }
	double getMass() { return 1.0 / _inverse_mass; }
	double getInverseMass() { return _inverse_mass; }
	double getDamping() { return _damping; }

	RenderItem* getRenderItem() { return _renderItem; }

	bool isAlive() { return _alive; }

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
#endif // !