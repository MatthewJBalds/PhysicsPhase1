#pragma once
#include <list>
#include "PhysicsParticle.h"
#include "ForceRegistry.h"
#include "GravityForceGenerator.h"

namespace Physics {

	class PhysicsWorld
	{
	public:
		ForceRegistry forceRegistry;

		//The list of all links
		std::list<PhysicsParticle*> Links;

		//The list of ALL our particles
		std::list<PhysicsParticle*> Particles;

		//Function to add particles to the list
		void AddParticle(PhysicsParticle* toAdd);

		//Universal update function to call the updates of All
		void Update(float time);

		void RemoveParticle(PhysicsParticle* particle) {
			Particles.remove(particle);
		}
	private:
		//Updates the particle list
		void UpdateParticleList();
		                                                                //-9.8f for gravity
		GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0,-9.8f , 0));

	};
}