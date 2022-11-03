#include "ForceGenerator.h"

GravityForceGenerator::GravityForceGenerator(const Vector3& gravity)
{
	_gravity = gravity;
	_name = "Gravity";
}

void GravityForceGenerator::updateForce(Particle* particle, double duration)
{
	if (fabs(particle->getInverseMass() < 1e-10))
		return;

	particle->addForce(_gravity * particle->getMass());
}
