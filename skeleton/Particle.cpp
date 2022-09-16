#include "Particle.h"

Particle::Particle(Vector3 position, Vector3 velocity)
{
	vel = velocity;
	//acc
	//dumping

	tr = PxTransform(position.x, position.y, position.z);
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(1.0)), &tr, { 0.5, 0, 0.5, 1 });
}

Particle::~Particle()
{
	DeregisterRenderItem(renderItem);
}


void Particle::integrate(double t)
{
	tr = PxTransform(tr.p.x + vel.x * t, tr.p.y + vel.y * t, tr.p.z + vel.z * t);
}
