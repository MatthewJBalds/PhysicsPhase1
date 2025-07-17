#include "ParticleSpring.h"

namespace Physics {
	void ParticleSpring::UpdateForce(PhysicsParticle* particle, float time) {
		MyVector pos = particle->Position;

		MyVector force = pos - otherParticle->Position;

		float mag = force.Magnitude();

		float springForce = -springConstant * abs(mag - restLength);

		//Direction = normalize
		force = force.Direction();

		force = force * springForce;

		particle->AddForce(force);
	}
}