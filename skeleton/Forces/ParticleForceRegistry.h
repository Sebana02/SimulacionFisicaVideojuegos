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

	void deleteParticleRegistry(Particle* const p) {
		auto it = begin();
		while (it != end()) {
			if (it->second == p) {
				it = erase(it);
			}
			else
				it++;
		}
	}


	void deleteForce(ForceGenerator* generator) {
		auto it = begin();
		while (it != end()) {
			if (it->first == generator)
				it = erase(it);
			else
				it++;
		}

	}
};

#endif //!__PARTICLE_FORCE_REGITRY__
