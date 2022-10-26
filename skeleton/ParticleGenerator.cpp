#include "ParticleGenerator.h"

ParticleGenerator::~ParticleGenerator()
{
	delete _model;
	_model = nullptr;
}

void ParticleGenerator::setParticle(Particle* model)
{
	delete _model;
	_model = model;
}
GaussianParticleGenerator::GaussianParticleGenerator(Vector3 mean_pos, Vector3 mean_vel,
	Vector3 dev_pos, Vector3 dev_vel, int num_particles, double prob)
{
	_mean_pos = mean_pos;
	_mean_vel = mean_vel;
	std_dev_pos = dev_pos;
	std_dev_vel = dev_vel;
	_num_particles = num_particles;
	_generation_probability = prob;
}

std::list<Particle*> GaussianParticleGenerator::generateParticles()
{
	std::list<Particle*> lista;

	if (_model == nullptr)
		return lista;

	for (int i = 0; i < _num_particles; i++) {

		if (abs(d(_gen)) > _generation_probability)
			continue;

		auto p = _model->clone();

		Vector3 newPos = _mean_pos;
		newPos.x += (d(_gen) * std_dev_pos.x);
		newPos.y += (d(_gen) * std_dev_pos.y);
		newPos.z += (d(_gen) * std_dev_pos.z);

		Vector3 newVel = _mean_vel;
		newVel.x += (d(_gen) * std_dev_vel.x);
		newVel.y += (d(_gen) * std_dev_vel.y);
		newVel.z += (d(_gen) * std_dev_vel.z);

		p->setVel(newVel);
		p->setPos(newPos);
		lista.push_back(p);

	}

	return lista;
}

UniformParticleGenerator::UniformParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, double a, double b,
	int num_particles, double prob)
{
	_mean_pos = mean_pos;
	_mean_vel = mean_vel;
	_num_particles = num_particles;
	_generation_probability = prob;

	_a = a;
	_b = b;

	d = std::uniform_real_distribution<double>{ _a,_b };
}

std::list<Particle*> UniformParticleGenerator::generateParticles()
{
	std::list<Particle*> lista;

	if (_model == nullptr)
		return lista;

	for (int i = 0; i < _num_particles; i++) {

		if (abs(d(_gen)) > _generation_probability)
			continue;

		auto p = _model->clone();

		Vector3 newPos = _mean_pos;
		newPos.x += d(_gen);
		newPos.y += d(_gen);
		newPos.z += d(_gen);

		Vector3 newVel = _mean_vel;
		newVel.x += d(_gen);
		newVel.y += d(_gen);
		newVel.z += d(_gen);

		p->setVel(newVel);
		p->setPos(newPos);
		lista.push_back(p);
	}
		return lista;
}

CircleParticleGenerator::CircleParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, Vector3 dev_pos, Vector3 dev_vel, int num_particles, double prob)
	: GaussianParticleGenerator(mean_pos,mean_vel,dev_pos,dev_vel,num_particles,prob)
{
}

std::list<Particle*> CircleParticleGenerator::generateParticles()
{
	std::list<Particle*> lista;

	if (_model == nullptr)
		return lista;

	for (int i = 0; i < _num_particles; i++) {

		if (abs(d(_gen)) > _generation_probability)
			continue;

		auto p = _model->clone();

		double increase = 360.0/_num_particles;

		Vector3 newVel = _mean_vel;
		newVel.y = (newVel.y * (std::sin(i * increase * std::_Pi / 180))) + (d(_gen) * std_dev_vel.y);
		newVel.z = (newVel.z * (std::cos(i * increase * std::_Pi / 180))) + (d(_gen) * std_dev_vel.z);

		p->setVel(newVel);
		p->setPos(_mean_pos);
		lista.push_back(p);

	}

	return lista;
}
