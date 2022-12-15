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
	Rigidbody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes, PxScene* gScene, PxPhysics* gPhysics);
	~Rigidbody();

	void integrate(double t);
	bool isAlive() noexcept { return _alive; };
protected:
	PxRigidDynamic* solid = nullptr;
	RenderItem* _renderItem = nullptr;
	PxShape* shape = nullptr;
	PxScene* _gScene = nullptr;
	PxPhysics* _gPhysics = nullptr;

	int _lifeTime, _duration;
	double _lifePos;

	bool _alive;
};
#endif // !1

