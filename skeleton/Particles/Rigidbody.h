#ifndef __RIGIDBODY__
#define __RIGIDBODY__

#include "../core.hpp"
#include "../RenderUtils.hpp"
#include <memory>
#include <list>

#include "../checkML.h"

using namespace std;
using namespace physx;

class Rigidbody
{
	
public:
	
	enum type{
		ARRIVING_PAINT = 0,
		TO_STOP,
		CANVAS,
		STATIC_PAINT,
		TO_DELETE,
		ERASING_PAINT,
	}_type;
	
	Rigidbody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, PxScene* gScene, PxPhysics* gPhysics, bool is_static,bool sphere,type t);
	virtual ~Rigidbody();

	void integrate(double t);
	bool isAlive() noexcept { return _alive; };
	PxRigidActor* getActor() noexcept{ return _solid; };
	PxShape* getShape() noexcept { return _shape; };
	Rigidbody* clone();

	void onCollision(type t);
protected:
	PxRigidActor* _solid = nullptr;
	PxShape* _shape = nullptr;
	RenderItem* _renderItem = nullptr;
	PxScene* _gScene = nullptr;
	PxPhysics* _gPhysics = nullptr;

	int _lifeTime, _duration;
	double _lifePos, _posDes;

	bool _alive;
	bool _static;
	bool _sphere;

	Vector3 _size;
};
#endif // !1

