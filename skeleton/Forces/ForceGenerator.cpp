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

WindForceGenerator::WindForceGenerator(double k1, double k2, const Vector3& wind, double radius, const Vector3& pos) :
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

	if ((pow(particleDistance.x, 2) <= pow(_position.x + _action_radius, 2))
		&& (pow(particleDistance.y, 2) <= pow(_position.y + _action_radius, 2))
		&& (pow(particleDistance.z, 2) <= pow(_position.z + _action_radius, 2))) {

		//compute drag force
		Vector3 vel = particle->getVel() - _wind;
		float velocity_module = vel.normalize();
		Vector3 dragF;
		velocity_module = _k1 * velocity_module + _k2 * velocity_module * velocity_module;

		dragF = -vel * velocity_module;

		//apply drag force
		particle->addForce(dragF);
	}

}

WhirlwindForceGenerator::WhirlwindForceGenerator(double k1, double k2, int K, double radius, const Vector3& position) :
	WindForceGenerator(k1, k2, { 0.0,0.0,0.0 }, radius, position)
{
	_name = "Whirlwind";
	_K = K;
}

void WhirlwindForceGenerator::updateForce(Particle* particle, double duration)
{
	if (fabs(particle->getInverseMass() < 1e-10))
		return;

	Vector3 particleDistance = particle->getPos();

	if ((pow(particleDistance.x, 2) <= pow(_position.x + _action_radius, 2))
		&& (pow(particleDistance.y, 2) <= pow(_position.y + _action_radius, 2))
		&& (pow(particleDistance.z, 2) <= pow(_position.z + _action_radius, 2))) {

		//compute drag force
		_wind = _K * Vector3(-(particle->getPos().z - _position.z)
			, 50 - (particle->getPos().y - _position.y)
			, particle->getPos().x - _position.x);

		Vector3 v = particle->getVel() - _wind;
		float velocity_module = v.normalize();

		Vector3 dragF;
		velocity_module = _k1 * velocity_module + _k2 * velocity_module * velocity_module;

		dragF = -v * velocity_module;

		//apply drag force
		particle->addForce(dragF);
	}
}


ExplosionForceGenerator::ExplosionForceGenerator(int K, const Vector3& position, float const_explosion)
{
	_K = K;
	_position = position;
	_const_explosion = const_explosion;
	_radius = 0;
}

void ExplosionForceGenerator::updateForce(Particle* particle, double t)
{
	if (fabs(particle->getInverseMass() < 1e-10))
		return;


	Vector3 particleDistance = particle->getPos();

	double r = pow((particle->getPos().x - _position.x), 2)
		+ pow((particle->getPos().y - _position.y), 2)
		+ pow((particle->getPos().z - _position.z), 2);

	if (r < _radius) {
		Vector3 explosion_force = (_K / r) *
			Vector3(particle->getPos().x - _position.x,
				particle->getPos().y - _position.y,
				particle->getPos().z - _position.z) *
			std::exp(-t / _const_explosion);

		particle->addForce(explosion_force);
	}

	_radius += vel_expansion * t;
}

//------------------------------------------------------------------------------------------

SpringForceGenerator::SpringForceGenerator(double K, double resting_length, Particle* other)
{
	_K = K;
	_resting_length = resting_length;
	_other = other;
	_name = "Spring";
}

void SpringForceGenerator::updateForce(Particle* particle, double duration)
{
	Vector3 force = _other->getPos() - particle->getPos();

	const float length = force.normalize();
	const float delta_x = length - _resting_length;
	force *= delta_x * _K;

	particle->addForce(force);
}

AnchoredSpringFG::AnchoredSpringFG(double K, double resting_length, const Vector3& anchor)
	:SpringForceGenerator(K,resting_length,nullptr)
{
	_other = new Particle(anchor, { 0,0,0 }, { 0,0,0 }, 0, 1e6, { 1,0,0,1 }, 5, -1, -1);
	_other->getRenderItem()->shape = CreateShape(physx::PxBoxGeometry(3.0, 3.0, 3.0));
}
AnchoredSpringFG::~AnchoredSpringFG() 
{
	delete _other;
}
BungeeForceGenerator::BungeeForceGenerator(double K, double resting_length, Particle* other)
	:SpringForceGenerator(K, resting_length, other)
{
	_name = "Bungee";
}

void BungeeForceGenerator::updateForce(Particle* particle, double duration)
{
	Vector3 f = particle->getPos();
	f -= _other->getPos();

	//length
	float length = f.normalize();
	length = length - _resting_length;
	if (length <= 0.0f)
		return;

	//Resulting force
	f *= -(length * _K);
	particle->addForce(f);
}

//--------------------------------------------------------------------------------------------
BuoyancyForceGenerator::BuoyancyForceGenerator(float h, float V, float d)
{
	_height = h;
	_volume = V;
	_liquid_density = d;
	_gravity = 9.8;
	_name = "Buoyancy";
}

void BuoyancyForceGenerator::updateForce(Particle* particle, double duration)
{
	float h = particle->getPos().y;
	float h0 = _liquid_particle->getPos().y;

	Vector3 f(0, 0, 0);
	float immersed = 0.0;
	if (h - h0 > _height * 0.5)
		immersed = 0.0;
	else if (h0 - h > _height * 0.5)
		immersed = 1.0;
	else
		immersed = (h0 - h) / _height * 0.5;

	f.y = _liquid_density * _volume * immersed * _gravity;

	particle->addForce(f);
	
}
