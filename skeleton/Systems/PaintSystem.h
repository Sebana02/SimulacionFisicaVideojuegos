#ifndef __PAINT_SYSTEM__ 
#define __PAINT_SYSTEM__

#include "../core.hpp"
#include "../RenderUtils.hpp"
#include <memory>
#include <list>
#include "../Particles/Rigidbody.h"
#include "../Forces/ForceGenerator.h"
#include "../Forces/RigidBodyForceRegistry.h"
#include "RigiBodyGenerator.h"

#include "../checkML.h"

using namespace physx;
using namespace std;

class PaintSystem
{
public:
	PaintSystem(PxScene* gScene, PxPhysics* gPhysics);
	~PaintSystem();

	void update(double t);
	void addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes,bool isStatic);
	Vector4 randomColor();


protected:
	std::list<Rigidbody*> _rigidBodies;
	PxScene* _gScene;
	PxPhysics* _gPhysics;

	RigidBodyForceRegistry* _registry = nullptr;

	GaussianRBGenerator* pincel = nullptr;
	
};
	

#endif //!__PAINT_SYSTEM__

