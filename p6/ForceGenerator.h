#pragma once
#include "PhysicsParticle.h"

namespace Physics {
	class ForceGenerator {
	public:
		//will override later
		virtual void UpdateForce(PhysicsParticle* p, float time) {
			p->AddForce(MyVector(0, 0, 0));
		}
	};
}