#pragma once
#include "../PhysicsParticle.h"
#include "../ParticleContact.h"

namespace Physics {
	class ParticleLink {
	public:
		PhysicsParticle* particles[2];

		virtual ParticleContact* GetContact() { return nullptr; };

	protected:
		float CurrentLength();



	};
}