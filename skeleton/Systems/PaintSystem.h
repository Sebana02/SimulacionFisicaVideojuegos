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
	Rigidbody* addRigidBody(PxTransform tr, Vector3 vel, Vector3 size, Vector4 color, float mass, int life, double posDes,bool isStatic,bool sphere, Rigidbody::type t);
	Vector4 randomColor();

	inline void setPaint(int p) noexcept { paint = p; };
	inline void setEraser(int e) noexcept { eraser = e; };
	void clearCanvas();
	
	void changeColor(int n);

protected:
	std::list<Rigidbody*> _rigidBodies;
	PxScene* _gScene;
	PxPhysics* _gPhysics;

	RigidBodyForceRegistry* _registry = nullptr;

	GaussianRBGenerator* pincel = nullptr;
	GaussianRBGenerator* borrador = nullptr;
	WindForceGeneratorRB* clear = nullptr;
	Rigidbody* canvas = nullptr;

	bool paint = false;
	bool eraser = false;

	int _speed;
	Vector3 _dispersion;
};
	

#endif //!__PAINT_SYSTEM__

