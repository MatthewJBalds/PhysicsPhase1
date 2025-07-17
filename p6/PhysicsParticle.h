#pragma once

#include "MyVector.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Physics {

	class PhysicsParticle
	{

	public:
		MyVector AngularVelocity = MyVector(0, 0, 0);
		float AngularDampening = 0.9f;
		glm::mat4 Rotation = glm::mat4(1.0f);
		void AddForceAtPoint(MyVector force, MyVector p);

	protected:
		MyVector accumulatedTorque = MyVector(0, 0, 0);
		virtual float MomentOfInertia();

	public:
		//size of particle
		float radius = 1.f;
		//restitution of particle how bouncy 0-1
		float restitution = 1.f;
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

	/*public:
		PhysicsParticle(const MyVector& position, float mass, float radius);*/


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

