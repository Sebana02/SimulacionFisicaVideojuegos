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


WindForceGenerator::WindForceGenerator(const Vector3& wind, double radius, const Vector3& pos)
{
	_wind = wind;
	_action_radius = radius;
	_position = pos;
	_name = "Wind";


	//dibuja el area de accion del viento
	PxTransform tr = PxTransform(_position.x, _position.y, _position.z);
	_renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(radius)), &tr, {1.0,0.0,0.0,0.1});
	DeregisterRenderItem(_renderItem);
}
WindForceGenerator::~WindForceGenerator() {
	DeregisterRenderItem(_renderItem);
	delete _renderItem;
}

void WindForceGenerator::updateForce(Particle* particle, double duration)
{
	if (fabs(particle->getInverseMass() < 1e-10))
		return;

	float k1 = 0.8;
	
	Vector3 particleDistance = particle->getPos();
	
	if ((particleDistance.x <= _position.x + _action_radius && particleDistance.x >= _position.x - _action_radius)
		&& (particleDistance.y <= _position.y + _action_radius && particleDistance.y >= _position.y - _action_radius)
		&& (particleDistance.z <= _position.z + _action_radius && particleDistance.z >= _position.z - _action_radius)) {
		
		particle->addForce(k1 * (_wind - particle->getVel()));
	}

}
