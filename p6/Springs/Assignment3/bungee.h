#pragma once
#include "../../ForceGenerator.h"
#include "../../PhysicsParticle.h"
#include "../../MyVector.h"
/*
The bungee class makes a bungee cord that streches and applies force to the particle
to spring it up again similar to how a bungee cord works in real life.
It applies a force to the particle when it is stretched beyond its rest length.
*/

namespace Physics {
	class Bungee : public ForceGenerator {

	private:
		float bunlen;
		float restLength;

	public:
		MyVector anchorPoint;
		float damping;

		Bungee(MyVector pos, float bungeeConst, float restLen)
			: anchorPoint(pos), bunlen(bungeeConst), restLength(restLen) {
		}

		void UpdateForce(PhysicsParticle* particle, float time) override;

	};
}
