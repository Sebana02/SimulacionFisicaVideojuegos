#ifndef __PARTICLE__ 
#define __PARTICLE__ 

#include "core.hpp"
#include "RenderUtils.hpp"

using namespace physx;

class Particle {

public:
	Particle(Vector3 position, Vector3 velocity, Vector3 accceleration, double damp, double mass);
	~Particle();

	void integrate(double t);

	void setVelocity(Vector3 velocity) { vel = velocity; }
	void setAcceleration(Vector3 acceleration) { accel = acceleration; }
	void setDamping(double damp) { damping = damp; }
	void setMass(double mass) { inverse_mass = 1.0/mass; }
	void setPos(Vector3 pos) { tr.p = pos; }

	Vector3 getVel() { return vel; }
	PxTransform& getTr() { return tr; }
	Vector3 getAccel() { return accel; }
	double getMass() { return 1.0 / inverse_mass; }
	double getInverseMass() { return inverse_mass; }
	double getDamping() { return damping; }

	RenderItem* getRenderItem() { return renderItem; }


protected:
	RenderItem* renderItem;
	Vector3 vel;
	PxTransform tr; //pasarle a render item la direccion de esta
	Vector3 accel;
	double damping;
	double inverse_mass;
};

class Proyectile : public Particle {
public:
	enum TYPE {
		PISTOL = 0,
		ARTILLERY,
		FIREBALL,
		LASER
	};

	Proyectile(TYPE tipo, Vector3 pos, Vector3 dir);
	
protected:
	TYPE type;
	TYPE getType() { return type; }
	void setType(TYPE newT) { type = newT; }

};
#endif // !