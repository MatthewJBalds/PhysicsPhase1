#include "ParticleContact.h"

namespace Physics {

	void  ParticleContact::Resolve(float time) {
		//call resolve velocity
		ResolveVelocity(time);
	}
	float ParticleContact::GetSeparatingSpeed() {
		MyVector velocity = particles[0]->Velocity;
		if (particles[1])velocity -= particles[1]->Velocity;
		return velocity.Dot(contactNormal);
	}

	void ParticleContact::ResolveVelocity(float time) {
		float separatingSpeed = GetSeparatingSpeed();

		if (separatingSpeed > 0) return;

		float newSS = -restitution * separatingSpeed;
		float deltaSpeed = newSS - separatingSpeed;

		float totalMass = (float)1 / particles[0]->mass;
		if (particles[1]) totalMass += (float)1 / particles[1]->mass;

		//if mass ==0 and negative invalid
		if (totalMass <= 0) return;

		//mag of impulse vector
		float impulseMag = deltaSpeed / totalMass;
		MyVector Impulse = contactNormal * impulseMag;

		MyVector v_A = Impulse * ((float)1 / particles[0]->mass);
		particles[0]->Velocity = particles[0]->Velocity + v_A;

		if (particles[1]) {
			MyVector v_B = Impulse * ((float)1 / particles[1]->mass);
			particles[1]->Velocity = particles[1]->Velocity + v_B;
		}
	}
}