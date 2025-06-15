#pragma once
#include "ForceGenerator.h"

namespace Physics {
	class GravityForceGenerator : public ForceGenerator{
	private:
		MyVector Gravity = MyVector(0, -9.8f, 0);

	public:
		GravityForceGenerator(const MyVector gravity) : Gravity(gravity) {}
		void UpdateForce(PhysicsParticle* particle, float time) override;
	};
}
