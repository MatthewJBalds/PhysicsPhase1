#pragma once
#include "ForceGenerator.h"

namespace Physics {
	class DragForceGenerator : public ForceGenerator {
	private:

		//coefficient of friction
		float k1 = 0.74f;
		float k2 = 0.57f;

	public:
		DragForceGenerator(){}
		DragForceGenerator(float _k1, float _k2): k1(_k1), k2(_k2){}

		void UpdateForce(PhysicsParticle* particle, float time) override;
	};
}
