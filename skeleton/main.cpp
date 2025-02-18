#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>
#include <list>
#include <random>
#include <iostream>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "./Systems/ParticleGenerator.h"
#include "./Systems/PaintSystem.h"

#include "checkML.h"

using namespace physx;
using namespace std;

std::string display_text = "This is a test";

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;


PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
ContactReportCallback gContactReportCallback;
MyCollisionCallback* callback = NULL;
//systems
PaintSystem* paintSystem = nullptr;

// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	gMaterial = gPhysics->createMaterial(0.9f, 0.5f, 0.0f);
	//gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);

	callback = new MyCollisionCallback();
	gScene->setSimulationEventCallback(callback);
	//create systems
	paintSystem = new PaintSystem(gScene, gPhysics);
}


// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	gScene->fetchResults(true);

	//update systems
	paintSystem->update(t);
	display_text = paintSystem->getInfo();

}

// Function to clean data
// Add custom code to the begining of the functioncout
void cleanupPhysics(bool interactive)
{
	//delete systems
	delete paintSystem;


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

	delete callback;
}


// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	if (paintSystem->isScreenshooting()) return;
	
	switch (toupper(key))
	{
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
		paintSystem->changeColor((key - '0'));
		break;
	case 'S':
		paintSystem->prepareScreenshot();
		break;
	case 'Q':
		paintSystem->setThickness(true);
		break;
	case 'W':
		paintSystem->setThickness(false);
		break;
	case 'A':
		paintSystem->deleteBodies();
		break;
	default:
		break;
	}
}

//Function called when a mouse button is clicked
void mousePress(int button, int state)
{
	if (paintSystem->isScreenshooting()) return;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		paintSystem->setPaint(!state);
		break;
	case GLUT_RIGHT_BUTTON:
		paintSystem->setEraser(!state);
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN) paintSystem->clearCanvas();
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