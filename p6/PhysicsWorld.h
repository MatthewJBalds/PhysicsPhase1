#pragma once
#include <list>
#include "PhysicsParticle.h"
#include "ForceRegistry.h"
#include "GravityForceGenerator.h"
#include <vector>
#include "ParticleContact.h"
#include "ContactResolver.h"
#include "Springs/ParticleLink.h"

namespace Physics {

	class PhysicsWorld
	{

	public:
		ForceRegistry forceRegistry;
		
		//The list of ALL our particles
		std::list<PhysicsParticle*> Particles;

		//Function to add particles to the list
		void AddParticle(PhysicsParticle* toAdd);

		//Universal update function to call the updates of All
		void Update(float time);

	/// <summary>
	/// CONTACTS
	/// </summary>
		
		//Dynamic Array for contacts
		//vectorclass
		std::vector<ParticleContact*> Contacts;
		void AddContact ( PhysicsParticle* p1, PhysicsParticle* p2, float restitution, MyVector contactNormal, float Depth);
		ContactResolver& GetContactResolver() { return contactResolver; }
		//The list of all links
		std::list<ParticleLink*> Links;

		void AddLink(ParticleLink* link);


	protected:

		ContactResolver contactResolver = ContactResolver(20);

		void GenerateContacts();

		void GetOverlaps();

	private:
		//Updates the particle list
		void UpdateParticleList();
		                                                                //-9.8f for gravity
		GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0, -9.8f, 0));

	};
}