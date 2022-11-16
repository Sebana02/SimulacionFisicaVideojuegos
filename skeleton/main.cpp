#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>
#include <list>
#include <random>
#include <iostream>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "Particle.h"
#include "ParticleGenerator.h"
#include "ParticleSystem.h"

#include "checkML.h"

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

ParticleSystem* _particle_system = nullptr;

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

	_particle_system = new ParticleSystem();
}


// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	gScene->fetchResults(true);

	_particle_system->update(t);
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

	delete _particle_system;
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch (toupper(key))
	{
	case '1':
	case '2':
	case '3':
	case '4'://proyectiles
		_particle_system->generateShot((Proyectile::PROYECTILE_TYPE)(key - '0' - 1), GetCamera()->getTransform().p, GetCamera()->getDir(), 5000, 200.0);
		break;
	case '5'://gaussian
	case '6'://uniform	
	{
		ParticleGenerator* p = _particle_system->getGenerator(key - '5');
		p->setActive(!p->isActive());
		break;
	}
	case '7': //uniform generation, just one time, no velocity
	{
		UniformParticleGenerator* p = new UniformParticleGenerator({ -100,50,0 }, { 0,0,0 }, -50, 50, 50, 10);
		int mass = rand() % 5 + 1;
		p->setParticle(new Particle({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 0.99, mass, _particle_system->randomColor(), mass, -1, 500));

		std::list<Particle*> particles = p->generateParticles();
		for (Particle* p : particles)
			p->setVel({ 0,0,0 });

		_particle_system->addParticles(particles);
		delete p;
		
		break;
	}
	case '8'://firework
		_particle_system->changeSpawnFireworks();
		break;
	case 'G':
		_particle_system->addGravity();
		break;
	case 'H':
		_particle_system->deleteGravity();
		break;
	case 'J':
		_particle_system->addWind();
		break;
	case 'K':
		_particle_system->deleteWind();
		break;
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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);//check memory leaks
	srand(time(NULL));//change seed for random numbers

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