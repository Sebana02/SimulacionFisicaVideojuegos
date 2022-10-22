#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>
#include <list>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include <iostream>

#include "Particle.h"
#include "ParticleGenerator.h"

using namespace physx;
using namespace std;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;


PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
ContactReportCallback gContactReportCallback;

vector<Proyectile*> shot;
Particle* suelo;
GaussianParticleGenerator* gaussian_generator;
UniformParticleGenerator* uniform_generator;
list<Particle*> particles_list;


// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);

	//suelo
	suelo = new Particle({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 0, 0, { 1.0,0.0,0.0,1.0 }, 1.0, 0, 0);
	RenderItem* renderItem = suelo->getRenderItem();
	renderItem->shape = CreateShape(physx::PxBoxGeometry(100.0f, 1.0f, 100.0f));

	//generador
	gaussian_generator = new GaussianParticleGenerator({ 0.0,0.0,0.0 }, { 1.0,25.0,1.0 }, { 0.0,0.0,0.0 }, { 2.0,1.0,2.0 }, 100, 2);
	gaussian_generator->setParticle(new Particle({ 0,0,0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.99, 1.0, { 0.5,0.5,1.0,1.0 }, 0.5, 10000, 500.0));

	uniform_generator = new UniformParticleGenerator({ 0,0,0 }, { 5,5,5 }, -10, 10);
	uniform_generator->setParticle(new Particle({ 0,0,0 }, { 0,0,0 }, { 0,-9.8,0 }, 0.99, 1.0, { 1.0,1.0,0.0,1.0 }, 1.0, 5000, 500.0));

}


// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	gScene->fetchResults(true);

	//proyectiles
	for (size_t i = 0; i < shot.size(); i++) {

		if (shot[i]->isAlive())
			shot[i]->integrate(t);

		else {
			delete shot[i];
			shot.erase(shot.begin() + i);
			i--;
		}
	}

	//añadir particulas
	list<Particle*> add_particles = gaussian_generator->generateParticles();//change generator method here
	for (auto par : add_particles)
		particles_list.push_back(par);
	add_particles.clear();


	//update de las particulas
	std::list<Particle*>::iterator it = particles_list.begin();
	while (it != particles_list.end()) {
		Particle* p = *it;
		if (p->isAlive()) {
			p->integrate(t);
			++it;
		}
		else {
			it = particles_list.erase(it);
			delete p;
		}
	}
}

// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	// Rigid Body ++++++++++++++++++++++++++++++++++++++++++
	gScene->release();
	gDispatcher->release();
	// -----------------------------------------------------
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();

	gFoundation->release();

	for (auto e : shot)
		delete e;

	delete suelo;

	for (auto particle : particles_list)
		delete particle;
	particles_list.clear();

	delete gaussian_generator;
	delete uniform_generator;
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch (toupper(key))
	{
	case '1':
	{
		shot.push_back(new Proyectile(Proyectile::PISTOL, GetCamera()->getTransform().p, GetCamera()->getDir(), 5000, 200.0));
		break;
	}
	case '2':
	{
		shot.push_back(new Proyectile(Proyectile::ARTILLERY, GetCamera()->getTransform().p, GetCamera()->getDir(), 5000, 200.0));
		break;
	}
	case '3':
	{
		shot.push_back(new Proyectile(Proyectile::FIREBALL, GetCamera()->getTransform().p, GetCamera()->getDir(), 5000, 200.0));
		break;
	}
	case '4':
	{
		shot.push_back(new Proyectile(Proyectile::LASER, GetCamera()->getTransform().p, GetCamera()->getDir(), 5000, 200.0));
		break;
	}
	default:
		break;
	}
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
	PX_UNUSED(actor1);
	PX_UNUSED(actor2);
}


int main(int, const char* const*)
{
#ifndef OFFLINE_EXECUTION 
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for (PxU32 i = 0; i < frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}