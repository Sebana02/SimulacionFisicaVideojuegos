#ifndef __PARTICLE__ 
#define __PARTICLE__ 

#include "core.hpp"
#include "RenderUtils.hpp"

using namespace physx;

class Particle {

public:
	Particle(Vector3 position, Vector3 velocity);
	~Particle();

	void integrate(double t);


private:
	RenderItem* renderItem;
	Vector3 vel;
	PxTransform tr; //pasarle a render item la direccion de esta
};
#endif // !