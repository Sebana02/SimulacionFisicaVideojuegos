#include "ForceGenerator.h"
#include <iostream>

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

//-----------------------------------------------------------------------------

ParticleDragGenerator::ParticleDragGenerator(const float k1, const float k2)
{
	_k1 = k1;
	_k2 = k2;
	_name = "Drag";
}

void ParticleDragGenerator::updateForce(Particle* particle, double duration)
{
	if (fabs(particle->getInverseMass() < 1e-10))
		return;

	//Compute the drag force
	Vector3 vel = particle->getVel();
	float drag_coef = vel.normalize();
	Vector3 dragF;
	drag_coef = _k1 * drag_coef + _k2 * drag_coef * drag_coef;
	dragF = -vel * drag_coef;

	//Apply the drag force
	particle->addForce(dragF);
}

//-----------------------------------------------------------------------------

WindForceGenerator::WindForceGenerator(double k1, double k2, const Vector3& wind, double radius, const Vector3& pos):
	ParticleDragGenerator(k1, k2)
{
	_wind = wind;
	_action_radius = radius;
	_position = pos;
	_name = "Wind";

	//dibuja el area de accion del viento
	_particle = new Particle(pos, { 0,0,0 }, { 0,0,0 }, 0, 0, { 1.0,0.0,0.0,0.1 }, radius, -1, -1);
}

WindForceGenerator::~WindForceGenerator() {
	delete _particle;
}

void WindForceGenerator::updateForce(Particle* particle, double duration)
{
	if (fabs(particle->getInverseMass() < 1e-10))
		return;


	Vector3 particleDistance = particle->getPos();

	if ((particleDistance.x <= _position.x + _action_radius && particleDistance.x >= _position.x - _action_radius)
		&& (particleDistance.y <= _position.y + _action_radius && particleDistance.y >= _position.y - _action_radius)
		&& (particleDistance.z <= _position.z + _action_radius && particleDistance.z >= _position.z - _action_radius)) {

		//compute drag force
		Vector3 vel =particle->getVel() - _wind;//?????
		cout << particle->getVel().y << endl;
		float velocity_module = vel.normalize();
		Vector3 dragF;
		velocity_module = _k1 * velocity_module + _k2 * velocity_module * velocity_module;

		dragF = -vel * velocity_module;

		//apply drag force
		particle->addForce(dragF);
	}
	
}

Whirlwind::Whirlwind(double k1, double k2, const Vector3& wind, double radius, const Vector3& position):
	WindForceGenerator(k1, k2, wind, radius, position)
{
	_name = "Whirlwind";
}

void Whirlwind::updateForce(Particle* particle, double duration)
{
	if (fabs(particle->getInverseMass() < 1e-10))
		return;
	
	//compute drag force
	auto x = -(particle->getPos().z - _position.z);
	auto y = 50 - (particle->getPos().y - _position.y);
	auto z = -(particle->getPos().x - _position.x);
	_wind = K * Vector3(x, y, z);
	
	Vector3 v = _wind - particle->getVel();
	float velocity_module = v.normalize();

	Vector3 dragF;
	velocity_module = _k1 * velocity_module + _k2 * velocity_module * velocity_module;

	dragF = -v * velocity_module;
	
	//apply drag force
	particle->addForce(dragF);
}
