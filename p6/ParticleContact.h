#pragma once

#include "PhysicsParticle.h"

namespace Physics {
	class ParticleContact {
	public:
		//collding particles
		PhysicsParticle* particles[2];
		//holds the coefficient of restitution
		float restitution;
		//contact normal of collision
		MyVector contactNormal;
		//resolve ocntact
		void Resolve(float time);

		float Depth;

		float GetSeparatingSpeed();


	protected:
		
		void ResolveVelocity(float time);

		void ResolveInterpenetration(float time);
	};
}
