#include "Particle.h"
#include "ParticleGenerator.h"

Particle::Particle(Vector3 position, Vector3 velocity, Vector3 accceleration, double damp, double mass,
	Vector4 color, double scale, int lifeTime, double posDes)
{
	_vel = velocity;
	_accel = accceleration;
	_damping = damp;
	_inverse_mass = 1.0 / mass;

	_tr = PxTransform(position.x, position.y, position.z);
	_renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(scale)), &_tr, color);

	_lifeTime = glutGet(GLUT_ELAPSED_TIME) + lifeTime;

	(posDes > 0 ? _lifePos = abs(_tr.p.magnitude()) + posDes : _lifePos = -1);

	_alive = true;

	_color = color;
	_scale = scale;
}

Particle::~Particle()
{
	DeregisterRenderItem(_renderItem);
}

void Particle::integrate(double t)
{

	if (_inverse_mass <= 0.0f) return;

	_vel += _accel * t;

	_vel *= powf(_damping, t);

	_tr = PxTransform(_tr.p.x + _vel.x * t, _tr.p.y + _vel.y * t, _tr.p.z + _vel.z * t);


	if (glutGet(GLUT_ELAPSED_TIME) >= _lifeTime ||
		(_lifePos > 0 && (abs(_tr.p.magnitude()) > _lifePos || _tr.p.y < 0)))
		_alive = false;
}

//___________________________________________
Proyectile::Proyectile(TYPE tipo, Vector3 pos, Vector3 dir, int lifeTime, double posDes) :
	Particle(pos, { 0,0,0 }, { 0,0,0 }, 0, 0, { 1.0,1.0,1.0,1.0 }, 1.0, lifeTime, posDes) {

	_type = tipo;

	switch (_type) {
	case PISTOL:
		_inverse_mass = 1.0 / 2.0f;
		_vel = dir * 35.0f;
		_accel = { 0.0f,-1.0f,0.0f };
		_damping = 0.99f;
		_renderItem->shape = CreateShape(physx::PxSphereGeometry(2.0));
		_renderItem->color = { 1.0,1.0,0.0,1.0 };
		break;
	case ARTILLERY:
		_inverse_mass = 1.0 / 200.0f;
		_vel = dir * 40.0f;
		_accel = { 0.0f,-20.0f,0.0f };
		_damping = 0.99f;
		_renderItem->shape = CreateShape(physx::PxSphereGeometry(5.0));
		_renderItem->color = { 1.0,0.5,0.5,1.0 };
		break;
	case FIREBALL:
		_inverse_mass = 1.0 / 1.0f;
		_vel = dir * 10.0f;
		_accel = { 0.0f,6.0f,0.0f };
		_damping = 0.9f;
		_renderItem->shape = CreateShape(physx::PxSphereGeometry(1.0));
		_renderItem->color = { 1.0,0.0,1.0,1.0 };
		break;
	case LASER:
		_inverse_mass = 1.0 / 0.1f;
		_vel = dir * 100.0f;
		_accel = { 0.0f,0.0f,0.0f };
		_damping = 0.99f;
		_renderItem->shape = CreateShape(physx::PxSphereGeometry(0.5));
		_renderItem->color = { 0.0,1.0,0.0,1.0 };
		break;
	default:
		break;
	}
}


Firework::Firework(Vector3 pos, Vector3 vel, Vector3 accel, std::list<std::shared_ptr<ParticleGenerator>> gens,
	double damp, double duration, unsigned type) :Particle(pos, vel, accel, damp, 1.0, { 1,1,1,1 }, 1.0, duration + glutGet(GLUT_ELAPSED_TIME), -1)
{
	_gens = gens;
	Particle::_type = FIREWORK;
	_type = type;
}

void Firework::integrate(double t)
{
	Particle::integrate(t);
}

Particle* Firework::clone() const
{
	return new Particle(_tr.p, _vel, _accel, _damping, _inverse_mass / 1.0, _color, _scale, _lifeTime, _lifePos);
}

std::list<Particle*> Firework::explode()
{
	std::list<Particle*> ret_val;

	for (auto& gen : _gens) {
		gen->setOrigin(_tr.p);
		std::list<Particle*> n_p = gen->generateParticles();

		for (Particle* p : n_p) {
			p->setVel(p->getVel() + _vel);
			ret_val.push_back(p);
		}
	}

	return ret_val;
}
