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
