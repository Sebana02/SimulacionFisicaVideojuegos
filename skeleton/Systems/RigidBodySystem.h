#ifndef __RIGID_BODY_SYSTEM__ 
#define __RIGID_BODY_SYSTEM__

#include "../core.hpp"
#include "../RenderUtils.hpp"
#include <memory>
#include <list>
#include "../Particles/Rigidbody.h"
#include "../Forces/ForceGenerator.h"
#include "../Forces/RigidBodyForceRegistry.h"

#include "../checkML.h"

using namespace physx;
using namespace std;

class RigidBodySystem
{
public:
	RigidBodySystem(PxScene* gScene, PxPhysics* gPhysics);
	~RigidBodySystem();

	void update(double t);
	void addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes,bool isStatic);
	Vector4 randomColor();

protected:
	std::list<Rigidbody*> _rigidBodies;
	PxScene* _gScene;
	PxPhysics* _gPhysics;

	int _max_bodies;
	int _spawn_delay;

	RigidBodyForceRegistry* _registry = nullptr;
	WindForceGeneratorRB* _wind = nullptr;
	
};
	

#endif //!__RIGID_BODY_SYSTEM__

