#include "RigiBodyGenerator.h"

RigidBodyGenerator::~RigidBodyGenerator()
{
	delete _model;
	_model = nullptr;
}

void RigidBodyGenerator::setParticle(Rigidbody* model)
{
	delete _model;
	model->getActor()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	_model = model;
}


GaussianRBGenerator::GaussianRBGenerator(Vector3 mean_pos, Vector3 mean_vel, Vector3 dev_pos, Vector3 dev_vel, int num_particles, double prob)
{
	_mean_pos = mean_pos;
	_mean_vel = mean_vel;
	std_dev_pos = dev_pos;
	std_dev_vel = dev_vel;
	_num_particles = num_particles;
	_generation_probability = prob;
}

std::list<Rigidbody*> GaussianRBGenerator::generateParticles()
{
	std::list<Rigidbody*> lista;

	if (_model == nullptr || !_active)
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

		static_cast<physx::PxRigidDynamic*>(p->getActor())->setLinearVelocity(newVel);
		p->getActor()->setGlobalPose(PxTransform(newPos));		
		lista.push_back(p);

	}

	return lista;
}
