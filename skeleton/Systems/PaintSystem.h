#ifndef __PAINT_SYSTEM__ 
#define __PAINT_SYSTEM__

#include "../core.hpp"
#include "../RenderUtils.hpp"
#include <memory>
#include <list>
#include <math.h>

#include "../Particles/Rigidbody.h"
#include "../Particles/Particle.h"
#include "../Forces/ForceGenerator.h"
#include "../Forces/RigidBodyForceRegistry.h"
#include "RigiBodyGenerator.h"
#include "ParticleGenerator.h"

#include "../checkML.h"

using namespace physx;
using namespace std;

class PaintSystem
{
public:
	PaintSystem(PxScene* gScene, PxPhysics* gPhysics);
	~PaintSystem();

	void update(double t);
	Vector4 randomColor();
	float randomMass();

	inline void setPaint(int p) noexcept { _paint = p; };
	inline void setEraser(int e) noexcept { _eraser = e; };
	void clearCanvas();
	void setThickness(bool thicker);
	void changeColor(int n);
	void deleteBodies();

	//screeshot
	void prepareScreenshot();
	void takeScreenshot();
	void generateFireworksSystem();
	inline bool isScreenshooting() noexcept{ return _photoTimer != -1; };

	
	//info en pantalla
	std::string getInfo() noexcept;

protected:
	std::list<Rigidbody*> _rigidBodies;
	PxScene* _gScene;
	PxPhysics* _gPhysics;

	RigidBodyForceRegistry* _registry = nullptr;

	GaussianRBGenerator* _pincel = nullptr;
	GaussianRBGenerator* _borrador = nullptr;
	WindForceGeneratorRB* _clear = nullptr;
	WhirlwindForceGeneratorRB* _clear2 = nullptr;
	Rigidbody* _canvas = nullptr;

	bool _paint;
	bool _eraser;

	int _speed;
	Vector3 _size;
	double _lifePos;
	float _maxThickness;
	Vector4 _color;

	//screenshot
	int _photoDelay;
	int _photoTimer;
	std::vector<Firework*> _fireworks_pool;
	ParticleGenerator* _firework_gen = nullptr;
	std::list<Particle*> _particles; 
};
	

#endif //!__PAINT_SYSTEM__

