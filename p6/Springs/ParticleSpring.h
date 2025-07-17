#pragma once
#include "../ForceGenerator.h"
#include "../PhysicsParticle.h"

namespace Physics {
	class ParticleSpring : public ForceGenerator {

	private:
		PhysicsParticle* otherParticle;
		float springConstant;
		float restLength;

	public:
		ParticleSpring(PhysicsParticle* particle, float _springConst, float _restLen) :
			otherParticle(particle), springConstant(_springConst), restLength(_restLen){}

		void UpdateForce(PhysicsParticle* particle, float time)override;


	};
}