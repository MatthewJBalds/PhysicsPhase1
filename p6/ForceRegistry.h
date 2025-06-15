#pragma once

#include "PhysicsParticle.h"
#include "ForceGenerator.h"

#include "list"

namespace Physics {
	class ForceRegistry {


	public:
		void Add(PhysicsParticle* particle, ForceGenerator* generator);
		void Remove(PhysicsParticle* particle, ForceGenerator* generator);
		void Clear();
		void UpdateForces(float time);

	protected:
		struct ParticleForceRegistry {
			PhysicsParticle* particle;
			ForceGenerator* generator;
		};

		std::list<ParticleForceRegistry> Registry;
	};
}
