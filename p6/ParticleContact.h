#pragma once

#include "PhysicsParticle.h"
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <iomanip>


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

		float clamp(float value, float min, float max) {
			return (value < min) ? min : (value > max) ? max : value;
		}

		

	protected:
		
		void ResolveVelocity(float time);

		void ResolveInterpenetration(float time);
	};
}
