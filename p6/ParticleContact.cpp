#include "ParticleContact.h"

namespace Physics {

	void  ParticleContact::Resolve(float time) {
		//call resolve velocity
		ResolveVelocity(time);

		ResolveInterpenetration(time);
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

	void ParticleContact::ResolveInterpenetration(float time) {
		//if theyre not overlapping- skip
		if (Depth <= 0) return;

		//get tghe total mass of the collsion
		float totalMass = (float)1 / particles[0]->mass;
		if (particles[1]) totalMass += (float)1 / particles[1]->mass;

		//invalid collison if total mass is 0 or less
		if (totalMass <= 0) return;

		float totalMoveByMass = Depth / totalMass;

		MyVector moveByMass = contactNormal * totalMoveByMass;

		//get the change in position for a

		MyVector P_a = moveByMass * ((float)1 / particles[0]->mass);
		//translate a
		particles[0]->Position += P_a;

		if (particles[1]) {
			MyVector P_b = moveByMass * (-(float)1 / particles[1]->mass);

			particles[1]->Position += P_b;
		}
		//after assume Depth = 0
		Depth = 0;
	}
}