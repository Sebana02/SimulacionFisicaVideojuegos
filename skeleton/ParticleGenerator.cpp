#include "ParticleGenerator.h"

void ParticleGenerator::setParticle(Particle* model)
{
	delete _model;
	_model = model;
}

GaussianParticleGenerator::GaussianParticleGenerator(Vector3 mean_pos, Vector3 mean_vel,
	Vector3 dev_pos, Vector3 dev_vel, int num_particles, double prob)
{
	name_ = "Gaussian";
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
		auto p = _model;

		if (d(_gen) > _generation_probability) {//preguntar

			Vector3 newPos = _mean_pos;
			newPos.x += (d(_gen) * std_dev_pos.x);
			newPos.y += (d(_gen) * std_dev_pos.y);
			newPos.z += (d(_gen) * std_dev_pos.z);

			Vector3 newVel = _mean_vel;
			newVel.x += (d(_gen) * std_dev_vel.x);
			newVel.y += (d(_gen) * std_dev_vel.y);
			newVel.z += (d(_gen) * std_dev_vel.z);

			lista.push_back(new Particle(newPos, newVel, p->getAccel(), p->getDamping(), p->getMass(), p->getColor(), p->getScale(), p->getLifeTime(), p->getLifePos()));
		}
	}

	return lista;
}

UniformParticleGenerator::UniformParticleGenerator(Vector3 mean_pos, Vector3 mean_vel, double a, double b,
	int num_particles, double prob)
{
	name_ = "Uniform";
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
		auto p = _model;

		if (d(_gen) > _generation_probability) {//preguntar

			Vector3 newPos = _mean_pos;
			newPos.x += d(_gen);
			newPos.y += d(_gen);
			newPos.z += d(_gen);

			Vector3 newVel = _mean_vel;
			newVel.x += d(_gen);
			newVel.y += d(_gen);
			newVel.z += d(_gen);

			lista.push_back(new Particle(newPos, newVel, p->getAccel(), p->getDamping(), p->getMass(), p->getColor(), p->getScale(), p->getLifeTime(), p->getLifePos()));
		}
	}

	return lista;
}

