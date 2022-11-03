#ifndef __PARTICLE_FORCE_REGITRY__
#define __PARTICLE_FORCE_REGITRY__

#include <map>
#include "ForceGenerator.h"

typedef std::pair<ForceGenerator*, Particle*> FRPair;
class ParticleForceRegistry : public std::multimap<ForceGenerator*, Particle*>
{
public:
	void updateForces(double duration) {
		for (auto it = begin(); it != end(); it++) {
			it->first->updateForce(it->second, duration);
		}
	}

	inline void addRegistry(ForceGenerator* fg, Particle* p) {
		insert(FRPair(fg, p));
	}

	void deleteParticleRegistry(Particle* const p){
		for (auto it = begin(); it != end();it++)
			if (it->second == p) {
				erase(it--);
			}
	}
};

#endif //!__PARTICLE_FORCE_REGITRY__
