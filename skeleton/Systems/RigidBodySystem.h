#ifndef __RIGID_BODY_SYSTEM__ 
#define __RIGID_BODY_SYSTEM__


#include "../Particles/Rigidbody.h"

#include "../checkML.h"

using namespace physx;
using namespace std;

class RigidBodySystem
{
public:
	RigidBodySystem(PxScene* gScene, PxPhysics* gPhysics);
	~RigidBodySystem();

	void update(double t);
	void addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes);

protected:
	std::list<Rigidbody*> _rigidBodies;
	PxScene* _gScene;
	PxPhysics* _gPhysics;

	int _max_bodies;
	int _spawn_delay;
};
	

#endif //!__RIGID_BODY_SYSTEM__

