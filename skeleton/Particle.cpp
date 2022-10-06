#include "Particle.h"


Particle::Particle(Vector3 position, Vector3 velocity, Vector3 accceleration, double damp, double mass)
{
	vel = velocity;
	accel = accceleration;
	damping = damp;
	inverse_mass = 1.0 / mass;

	tr = PxTransform(position.x, position.y, position.z);
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(1.0)), &tr, { 0.5, 0, 0.5, 1 });

	startTime = glutGet(GLUT_ELAPSED_TIME);
}

Particle::~Particle()
{
	DeregisterRenderItem(renderItem);
}

void Particle::integrate(double t)
{
	if (inverse_mass <= 0.0f) return;

	tr = PxTransform(tr.p.x + vel.x * t, tr.p.y + vel.y * t, tr.p.z + vel.z * t);

	vel += accel * t;

	vel *= powf(damping, t);
}

//___________________________________________
Proyectile::Proyectile(TYPE tipo, Vector3 pos, Vector3 dir) : 
			Particle(pos, { 0,0,0 }, { 0,0,0 }, 0, 0) {

	type = tipo;

	switch (type) {
	case PISTOL:
		inverse_mass = 1.0 / 2.0f;
		//vel = { 0.0f,0.0f,35.0f };
		vel = dir * 35.0f;
		accel = { 0.0f,-1.0f,0.0f };
		damping = 0.99f;
		renderItem->shape = CreateShape(physx::PxSphereGeometry(2.0));
		renderItem->color = { 1.0,1.0,0.0,1.0 };
		break;
	case ARTILLERY:
		inverse_mass = 1.0 / 200.0f;
		//vel = { 0.0f,30.0f,40.0f };
		vel = dir * 40.0f;
		accel = { 0.0f,-20.0f,0.0f };
		damping = 0.99f;
		renderItem->shape = CreateShape(physx::PxSphereGeometry(5.0));
		renderItem->color = { 1.0,0.5,0.5,1.0 };
		break;
	case FIREBALL:
		inverse_mass = 1.0 / 1.0f;
		//vel = { 0.0f,0.0f,10.0f };
		vel = dir * 10.0f;
		accel = { 0.0f,6.0f,0.0f };
		damping = 0.9f;
		renderItem->shape = CreateShape(physx::PxSphereGeometry(1.0));
		renderItem->color = { 1.0,0.0,1.0,1.0 };
		break;
	case LASER:
		inverse_mass = 1.0 / 0.1f;
		//vel = { 0.0f,0.0f,100.0f };
		vel = dir * 100.0f;
		accel = { 0.0f,0.0f,0.0f };
		damping = 0.99f;
		renderItem->shape = CreateShape(physx::PxSphereGeometry(0.5));
		renderItem->color = {0.0,1.0,0.0,1.0 };
		break;
	default:
		break;
	}
}
