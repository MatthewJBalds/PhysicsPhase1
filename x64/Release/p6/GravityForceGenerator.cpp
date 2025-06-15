#include "GravityForceGenerator.h"

namespace Physics {
	void GravityForceGenerator::UpdateForce(PhysicsParticle* particle, float time) {
		if (particle->mass <= 0) return;

		//f =  A  *  m
		MyVector Force = Gravity * particle->mass;
		particle->AddForce(Force);
	}
}