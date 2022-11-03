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
	enum class TYPE {
		NORMAL = 0,
		FIREWORK
	}_type;

	Particle(Vector3 position, Vector3 velocity, Vector3 accceleration, double damp, double mass,
		Vector4 color, double scale, int lifeTime, double posDes);
	virtual ~Particle();

	void integrate(double t);

	inline RenderItem* getRenderItem() noexcept { return _renderItem; };
	inline bool isAlive() noexcept { return _alive; };
	
	inline Vector3 getVel() { return _vel; };
	inline void setVel(Vector3 vel) { _vel = vel; };
	inline void setPos(Vector3 pos) { _tr.p = pos; };
	inline double getInverseMass() noexcept { return _inverse_mass; };
	inline double getMass() noexcept { return _inverse_mass / 1.0; };
	
	virtual inline Particle* clone() const;

	//fuerzas
	inline void clearForce() {_force = { 0,0,0 };};
	inline void addForce(const Vector3& force) { _force += force; };
protected:
	RenderItem* _renderItem;
	Vector3 _vel;
	PxTransform _tr; //pasarle a render item la direccion de esta
	Vector3 _accel;
	double _damping;
	double _inverse_mass;

	//determinan la vida de la particula
	int _lifeTime, _duration;
	double _lifePos;

	//is alive?
	bool _alive;

	Vector4 _color;
	double _scale;

	Vector3 _force;
};

class Proyectile : public Particle {
public:
	enum class PROYECTILE_TYPE {
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
	inline Firework* clone()const override;
	std::list<std::shared_ptr <ParticleGenerator>> _gens;
	std::list<Particle*> explode();

};
#endif // !