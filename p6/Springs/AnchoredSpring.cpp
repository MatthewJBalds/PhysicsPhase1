#include "AnchoredSpring.h"

namespace Physics {
	void AnchoredSpring::UpdateForce(PhysicsParticle* particle, float time) {

		MyVector pos = particle->Position;

		MyVector force = pos - anchorPoint;

		float mag = force.Magnitude();

		float springForce = -springConstant * abs(mag - restLength);

		//Direction = normalize
		force = force.Direction();

		force = force * springForce;

		particle->AddForce(force);
	}
}