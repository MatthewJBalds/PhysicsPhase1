#pragma once

#include "MyVector.h"

namespace Physics {

	class PhysicsParticle
	{
	public:
		// mass of particle
		float mass = 0;
		//current pos of particle
		MyVector Position;
		//current velocity of particle
		MyVector Velocity;
		//currernt accel of particle
		MyVector Acceleration;
		//approx drag
		float Damping = 0.9f;

		void AddForce(MyVector force);

		void ResetForce();

	protected:
		//updates relative to time
		void UpdatePosition(float time);
		void UpdateVelocity(float time);

		//This if forced gather before fixed update
		//U U U U FU - Update 4X -> Fixed update
		MyVector AccumulatedForce = MyVector(0, 0, 0);
	public:
		//updates given time
		void Update(float time);

		//set destroy
		void Destroy();
		//get destroy
		bool IsDestroyed() {
			return isDestroyed;
		}

		//check at center
		bool AtCenter(float threshold = 0.1f) const {
			return (Position.x < threshold && Position.x > -threshold &&
				Position.y < threshold && Position.y > -threshold &&
				Position.z < threshold && Position.z > -threshold);
		}

	protected:
		//flag to hold when to destroy particle
		bool isDestroyed = false;

	};
}

