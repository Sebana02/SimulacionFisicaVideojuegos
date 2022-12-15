#ifndef __RIGIDBODY_FORCE_REGISTRY__
#define __RIGIDBODY_FORCE_REGISTRY__

#include <map>
#include "../Forces/ForceGenerator.h"

typedef std::pair<ForceGenerator*, Rigidbody*> FRBPAIR;
class RigidBodyForceRegistry :public std::multimap<ForceGenerator*,Rigidbody*> {
public:
	void updateForces(double duration) {
		for (auto it = begin(); it != end(); it++) {
			it->first->updateForce(it->second, duration);
		}
	}

	inline void addRegistry(ForceGenerator* fg, Rigidbody* p) {
		insert(FRBPAIR(fg, p));
	}

	void deleteBodyRegistry(Rigidbody* const p) {
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

#endif // !__RIGIDBODY_FORCE_REGISTRY__

