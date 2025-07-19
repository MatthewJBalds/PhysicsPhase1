#include "PhysicsWorld.h"

using namespace Physics;

void PhysicsWorld::AddParticle(PhysicsParticle* toAdd)
{
	Particles.push_back(toAdd);

	//affected by gravity immedietly
	forceRegistry.Add(toAdd, &Gravity);
}

void PhysicsWorld::Update(float time)
{
	//update list first
	UpdateParticleList();

	forceRegistry.UpdateForces(time);

	//create iterator dor the start of the list
	for (std::list<PhysicsParticle*>::iterator p = Particles.begin();
		//continue looping until end of the list
		p != Particles.end();
		//move to next particle
		p++
		)
	{
		(*p)->Update(time);
	}

	//Call Generate Contacts Here
	GenerateContacts();

	//only call resolve contacts when there are contacts
	if (Contacts.size() > 0) {
		contactResolver.ResolveContacts(Contacts, time);
	}

	//can remove
	//for (auto p : Particles) p->Velocity *= 0.99f;

}

void PhysicsWorld::UpdateParticleList() {
	//Removes all particles in the list that
	//return true to the function below

	Particles.remove_if(
		//checks all the particles int the list
		//if isDestroyed flag is true
		[](PhysicsParticle* p) {
			return p->IsDestroyed();
		}
	);
}

void PhysicsWorld::AddContact(PhysicsParticle* p1, PhysicsParticle* p2, float restitution, MyVector contactNormal, float Depth) {

	ParticleContact* toAdd = new ParticleContact();

	//assign the needed variables
	toAdd->particles[0] = p1;
	toAdd->particles[1] = p2;
	toAdd->restitution = restitution;
	toAdd->contactNormal = contactNormal;
	toAdd->Depth = Depth;

	Contacts.push_back(toAdd);
}

void PhysicsWorld::GenerateContacts() {
	//clear cur lest oc contacts
	Contacts.clear();

	//Get overlaps for collisions
	GetOverlaps();

	//iterate thru the list of links
	for (std::list<ParticleLink*>::iterator i = Links.begin();
		i != Links.end();
		i++) {
		ParticleContact* contact = (*i)->GetContact();

		if (contact != nullptr) {
			Contacts.push_back(contact);
		}
	}

}

void Physics::PhysicsWorld::GetOverlaps()
{//particles[i]
	for (int i = 0; i < Particles.size(); i++){
		std::list<PhysicsParticle*>::iterator a = std::next(Particles.begin(), i);
		//pairs for conparison a - b for comparison
		for(int h = i + 1; h < Particles.size(); h++ ) {
			std::list<PhysicsParticle*>::iterator b = std::next(Particles.begin(), h);
			//create vector from a->b
			MyVector mag2Vector = (*a)->Position - (*b)->Position;
			//get sqmagnitude
			float mag2 = mag2Vector.SqMagnitude();
			//mag < rad
			float rad = (*a)->radius + (*b)->radius;
			//get sqradius
			float rad2 = rad * rad;
			//if equal they are touching
			//if less they are overlapping
			if (mag2 <= rad2) {
				//there is collision  //Noramlize->deirectoin
				MyVector dir = mag2Vector.Direction();
				float r = rad2 - mag2;
				float Depth = sqrt(r); 

				//get the lesser restitution of the 2
				float restitution = fmin((*a)->restitution, (*b)->restitution);

				AddContact(*a, *b, restitution, dir, Depth);
			}
		}

	}
}

void PhysicsWorld::AddLink(ParticleLink* link) {
	Links.push_back(link);
}

