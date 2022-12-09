#ifndef __RIGID_BODY_SYSTEM__ 
#define __RIGID_BODY_SYSTEM__

#include "../core.hpp"
#include "../RenderUtils.hpp"
#include <memory>
#include <list>

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
	std::list<PxRigidDynamic*> _rigidBodies;
	PxScene* _gScene;
	PxPhysics* _gPhysics;

	int max_bodies;
};
	

#endif //!__RIGID_BODY_SYSTEM__

