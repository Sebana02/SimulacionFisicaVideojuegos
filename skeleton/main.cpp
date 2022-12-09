#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>
#include <list>
#include <random>
#include <iostream>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "./Particles/Particle.h"
#include "./Systems/ParticleGenerator.h"
#include "./Systems/ParticleSystem.h"
#include "./Systems/RigidBodySystem.h"

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
RigidBodySystem* _rigid_body_system = nullptr;

// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	gMaterial = gPhysics->createMaterial(0.9f, 0.5f, 0.7f);
	//gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);

	//RigidBody system
	//add static objects
	PxRigidStatic* Suelo = gPhysics->createRigidStatic(PxTransform({0.0f, 0.0f, 0.0f}));
	PxShape* shape = CreateShape(PxBoxGeometry(100, 0.1, 100));
	Suelo->attachShape(*shape);
	RenderItem* rI = new RenderItem(shape, Suelo, { 0.8,0.8,0.8,1.0 });
	gScene->addActor(*Suelo);
	
	PxRigidStatic* Pared = gPhysics->createRigidStatic(PxTransform({ 10,10,-30 }));
	PxShape* shape_suelo = CreateShape(PxBoxGeometry(40, 20, 5));
	Pared->attachShape(*shape_suelo);
	RenderItem* rI2 = new RenderItem(shape_suelo, Pared, { 0.8,0.8,0.8,1.0 });
	gScene->addActor(*Pared);
	
	_rigid_body_system = new RigidBodySystem(gScene, gPhysics);

	
	//particle system
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
	case '7': //gaussian generation, just one time, no velocity
	{
		GaussianParticleGenerator* p = new GaussianParticleGenerator({ 0,0,0 }, { 0,0,0 }, { 50,50,50 }, { 0,0,0 }, 10, 100);
		const int mass = rand() % 50 + 1;
		p->setParticle(new Particle({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 0.99, mass, _particle_system->randomColor(), mass / 5.0, -1, 2000));
		_particle_system->addParticles(p->generateParticles());
		delete p;

		break;
	}
	case '8': //uniform generation, just one time, with velocity
	{
		GaussianParticleGenerator* p = new GaussianParticleGenerator({ 0,0,0 }, { 1,5,1 }, { 50,50,50 }, { 1,5,1 }, 10, 100);
		int mass = rand() % 20 + 1;
		p->setParticle(new Particle({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 0.99, mass, _particle_system->randomColor(), mass / 5.0, -1, 2000));
		_particle_system->addParticles(p->generateParticles());
		cout << mass << endl;
		delete p;

		break;
	}
	case '9'://firework
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
	case 'V':
		_particle_system->addWhirlwind();
		break;
	case 'B':
		_particle_system->deleteWhirlwind();
		break;
	case 'N':
		_particle_system->addExplosion();
		break;
	case 'M':
		_particle_system->deleteExplosion();
		break;
	case 'P':
		_particle_system->generateSpringDemo();
		break;
	case 'O':
		_particle_system->generateAnchoredSpringDemo();
		break;
	case 'I':
		_particle_system->generateBungeeSpringDemo();
		break;
	case 'U':
		_particle_system->generateBuoyancyDemo();
		break;
	case 'Y':
		_particle_system->generateSlinkyDemo();
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