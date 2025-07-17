#pragma	once
#include "../../ForceGenerator.h"
#include "../../PhysicsParticle.h"
#include "../../MyVector.h"
/*
The chain class creates a chain that applies a force to the particle
to simulate the effect of a chain pulling it back to the anchor point.
This is similar to how a chain works in real life, where the chain's length
is fixed and it applies a force to the particle when it is stretched beyond its rest length.
*/

namespace Physics {
	class Chain : public ForceGenerator {

	private:
		float chainlen;
		float restLength;

	public:
		MyVector anchorPoint;

		Chain(MyVector pos, float _chainlen, float restLen)
			: anchorPoint(pos), chainlen(_chainlen), restLength(restLen) {
		}

		void UpdateForce(PhysicsParticle* particle, float time) override;
	};
}
